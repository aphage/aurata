/*

MIT License

Copyright(c) 2020 Misaka Mikoto(aphage) diyloli@outlook.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "Aurata.h"

#include "sqlite/sqlite3.h"

namespace aurata{

	thread_local std::stack<Connection*> local_conns;
	Aurata::Aurata(ConnectionPool* conn_pool):conn_pool(conn_pool) {
		
	}

	Aurata::~Aurata() {

	}

	Connection* Aurata::GetConnection(bool& is_local) {
		if (local_conns.empty()) {
			is_local = false;
			auto conn = this->conn_pool->GetConnection();
			local_conns.push(conn);
			return conn;
		}
		else {
			is_local = true;
			return local_conns.top();
		}
	}

	void Aurata::ReturnConnection(Connection* connection, bool& is_local) {
		if (!is_local) {
			this->conn_pool->ReturnConnection(connection);
			local_conns.pop();
		}
	}

	void Aurata::BindParams(sqlite3_stmt* stmt, std::vector<AurataParam*>&& params) {
		for (int i = 0; i < params.size(); i++) {
			{
				auto v = dynamic_cast<AurataParamInteger*>(params[i]);
				if (v != nullptr) {
					sqlite3_bind_int64(stmt, i + 1, v->v);
					continue;
				}
			}
			{
				auto v = dynamic_cast<AurataParamDouble*>(params[i]);
				if (v != nullptr) {
					sqlite3_bind_double(stmt, i + 1, v->v);
					continue;
				}
			}
			{
				auto v = dynamic_cast<AurataParamBlob*>(params[i]);
				if (v != nullptr) {
					sqlite3_bind_blob(stmt, i + 1, v->v.c_str(), v->v.length(), SQLITE_STATIC);
					continue;
				}
			}
			{
				auto v = dynamic_cast<AurataParamText*>(params[i]);
				if (v != nullptr) {
					sqlite3_bind_text(stmt, i + 1, v->v.c_str(), v->v.length(), SQLITE_STATIC);
					continue;
				}
			}
			{
				auto v = dynamic_cast<AurataParamNull*>(params[i]);
				if (v != nullptr) {
					sqlite3_bind_null(stmt, i + 1);
					continue;
				}
			}

		}
	}

	void Aurata::FreeParams(std::vector<AurataParam*>&& params) {
		for (int i = 0; i < params.size(); i++) {
			{
				auto v = dynamic_cast<AurataParamInteger*>(params[i]);
				if (v != nullptr) {
					delete v;
					continue;
				}
			}
			{
				auto v = dynamic_cast<AurataParamDouble*>(params[i]);
				if (v != nullptr) {
					delete v;
					continue;
				}
			}
			{
				auto v = dynamic_cast<AurataParamBlob*>(params[i]);
				if (v != nullptr) {
					delete v;
					continue;
				}
			}
			{
				auto v = dynamic_cast<AurataParamText*>(params[i]);
				if (v != nullptr) {
					delete v;
					continue;
				}
			}
			{
				auto v = dynamic_cast<AurataParamNull*>(params[i]);
				if (v != nullptr) {
					delete v;
					continue;
				}
			}
		}
	}

	Json::Value Aurata::ReadRow(sqlite3_stmt* stmt) {
		auto count = sqlite3_column_count(stmt);
		Json::Value row;

		for (int i = 0; i < count; i++) {
			auto name = sqlite3_column_name(stmt, i);
			auto type = sqlite3_column_type(stmt, i);
			switch (type) {
				case SQLITE_INTEGER: {
					row[name] = static_cast<int64_t>(sqlite3_column_int64(stmt, i));
				}break;
				case SQLITE_FLOAT: {
					row[name] = static_cast<double>(sqlite3_column_double(stmt, i));
				}break;
				case SQLITE_BLOB: {
					auto bytes = sqlite3_column_bytes(stmt, i);
					row[name] = std::string(static_cast<const char*>(sqlite3_column_blob(stmt, i)), bytes);
				}break;
				case SQLITE_NULL: {
					row[name] = Json::nullValue;
				}break;
				case SQLITE_TEXT: {
					auto bytes = sqlite3_column_bytes(stmt, i);
					row[name] = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)), bytes);
				}break;
			}
		}
		return row;
	}

	std::vector<Json::Value> Aurata::Select(std::string&& sql, std::vector<AurataParam*>&& params) {
		bool is_local = false;
		auto conn = this->GetConnection(is_local);
		
		std::vector<Json::Value> list;
		sqlite3_stmt* res = nullptr;
		auto rc = sqlite3_prepare_v2(reinterpret_cast<sqlite3*>(conn), sql.c_str(), sql.length(), &res, NULL);
		if (rc != SQLITE_OK) {
			this->ReturnConnection(conn, is_local);
			return std::move(list);
		}

		this->BindParams(res, std::move(params));

		while (sqlite3_step(res) == SQLITE_ROW) {
			list.push_back(std::move(this->ReadRow(res)));
		}
		sqlite3_finalize(res);

		this->FreeParams(std::move(params));

		this->ReturnConnection(conn, is_local);

		return std::move(list);
	}

	int Aurata::Update(std::string&& sql, std::vector<AurataParam*>&& params) {
		bool is_local = false;
		auto conn = this->GetConnection(is_local);

		sqlite3_stmt* res = nullptr;
		auto rc = sqlite3_prepare_v2(reinterpret_cast<sqlite3*>(conn), sql.c_str(), sql.length(), &res, NULL);
		if (rc != SQLITE_OK) {
			this->ReturnConnection(conn, is_local);
			return 0;
		}
		this->BindParams(res, std::move(params));
		int count = 0;

		if (sqlite3_step(res) == SQLITE_DONE) {
			count = sqlite3_changes(reinterpret_cast<sqlite3*>(conn));
		}
		sqlite3_finalize(res);
		this->FreeParams(std::move(params));
		this->ReturnConnection(conn, is_local);

		return count;
	}
	
	int64_t Aurata::Insert(std::string&& sql, std::vector<AurataParam*>&& params) {
		bool is_local = false;
		auto conn = this->GetConnection(is_local);

		this->Update(std::move(sql), std::move(params));
		auto auto_id = sqlite3_last_insert_rowid(reinterpret_cast<sqlite3*>(conn));
		this->ReturnConnection(conn, is_local);
		return auto_id;
	}

	bool Aurata::Transaction(std::function<bool()> f) {
		auto conn = this->conn_pool->GetConnection();
		local_conns.push(conn);

		this->Update("BEGIN TRANSACTION;");

		if (!f()) {
			this->Update("ROLLBACK;");
			this->conn_pool->ReturnConnection(conn);
			local_conns.pop();
			return false;
		}

		this->Update("COMMIT;");
		this->conn_pool->ReturnConnection(conn);
		local_conns.pop();
		return true;
	}

	bool Aurata::BackupDB(std::string&& file_name) {
		sqlite3* file = nullptr;
		auto rc = sqlite3_open(file_name.c_str(), &file);
		if (rc != SQLITE_OK) {
			return false;
		}
		bool r = false;
		auto conn = this->conn_pool->GetConnection();
		auto backup = sqlite3_backup_init(file, "main", reinterpret_cast<sqlite3*>(conn), "main");
		if (backup) {
			do {
				rc = sqlite3_backup_step(backup, 5);
				if(rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED){
					sqlite3_sleep(250);
				}
			} while (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
			sqlite3_backup_finish(backup);
			r = true;
		}
		this->conn_pool->ReturnConnection(conn);
		sqlite3_close(file);
		return r;
	}
}