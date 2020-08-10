#include "Aurata.h"

#include "sqlite/sqlite3.h"

namespace aurata{

	thread_local std::stack<Connection*> Aurata::connections = std::stack<Connection*>();
	Aurata::Aurata() {
		
	}

	Aurata::~Aurata() {

	}

	Connection* Aurata::GetConnection(bool& is_local) {
		if (!this->connections.empty()) {
			is_local = true;
			return this->connections.top();
		}

		is_local = false;
		return this->conn_manage.GetConnection();
	}

	void Aurata::FreeConnection(bool& is_local, Connection* connection) {
		if (!is_local) {
			this->conn_manage.FreeConnection(connection);
		}
	}

	std::vector<Json::Value> Aurata::Select(std::string&& sql, std::vector<AurataParam*>&& params) {
		bool is_local = false;
		auto conn = reinterpret_cast<sqlite3*>(this->GetConnection(is_local));
		std::vector<Json::Value> list;
		sqlite3_stmt* res = nullptr;
		auto rc = sqlite3_prepare_v2(conn, sql.c_str(), sql.length(), &res, NULL);
		if (rc != SQLITE_OK) {
			this->FreeConnection(is_local, reinterpret_cast<Connection*>(conn));
			return list;
		}

		for (int i = 0; i < params.size(); i++) {
			{
				auto v = dynamic_cast<AurataParamInteger*>(params[i]);
				if (v != nullptr) {
					sqlite3_bind_int64(res, i + 1, v->v);
					continue;
				}
			}
			{
				auto v = dynamic_cast<AurataParamDouble*>(params[i]);
				if (v != nullptr) {
					sqlite3_bind_double(res, i + 1, v->v);
					continue;
				}
			}
			{
				auto v = dynamic_cast<AurataParamBlob*>(params[i]);
				if (v != nullptr) {
					sqlite3_bind_blob(res, i + 1, v->v.c_str(), v->v.length(), SQLITE_STATIC);
					continue;
				}
			}
			{
				auto v = dynamic_cast<AurataParamText*>(params[i]);
				if (v != nullptr) {
					sqlite3_bind_text(res, i + 1, v->v.c_str(), v->v.length(), SQLITE_STATIC);
					continue;
				}
			}
			{
				auto v = dynamic_cast<AurataParamNull*>(params[i]);
				if (v != nullptr) {
					sqlite3_bind_null(res, i + 1);
					delete v;
					continue;
				}
			}

		}

		while (sqlite3_step(res) == SQLITE_ROW) {
			auto count = sqlite3_column_count(res);
			Json::Value data;
			for (int i = 0; i < count; i++) {
				auto name = sqlite3_column_name(res, i);
				auto type = sqlite3_column_type(res, i);
				switch (type) {
				case SQLITE_INTEGER: {
					data[name] = sqlite3_column_int64(res, i);
				}break;
				case SQLITE_FLOAT: {
					data[name] = sqlite3_column_double(res, i);
				}break;
				case SQLITE_BLOB: {
					auto bytes = sqlite3_column_bytes(res, i);
					data[name] = std::string((char*)sqlite3_column_blob(res, i), bytes);
				}break;
				case SQLITE_NULL: {
					data[name] = Json::nullValue;
				}break;
				case SQLITE_TEXT: {
					auto bytes = sqlite3_column_bytes(res, i);
					data[name] = std::string((char*)sqlite3_column_text(res, i), bytes);
				}break;
				}
			}
			list.push_back(std::move(data));
		}
		sqlite3_finalize(res);

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

		this->FreeConnection(is_local, reinterpret_cast<Connection*>(conn));

		return std::move(list);
	}

	int Aurata::Update(std::string&& sql, std::vector<AurataParam*>&& params) {
		bool is_local = false;
		auto conn = reinterpret_cast<sqlite3*>(this->GetConnection(is_local));

		if (!is_local) {
			this->connections.push(reinterpret_cast<Connection*>(conn));
		}

		this->Select(std::move(sql), std::move(params));
		auto count = sqlite3_changes(conn);

		if (!is_local) {
			this->connections.pop();
		}
		this->FreeConnection(is_local, reinterpret_cast<Connection*>(conn));

		return count;
	}
	
	int Aurata::Insert(std::string&& sql, std::vector<AurataParam*>&& params, int64_t* auto_id) {
		bool is_local = false;
		auto conn = reinterpret_cast<sqlite3*>(this->GetConnection(is_local));

		if (!is_local) {
			this->connections.push(reinterpret_cast<Connection*>(conn));
		}

		this->Select(std::move(sql), std::move(params));
		auto count = sqlite3_changes(conn);

		if (auto_id != nullptr) {
			*auto_id = sqlite3_last_insert_rowid(conn);
		}

		if (!is_local) {
			this->connections.pop();
		}
		this->FreeConnection(is_local, reinterpret_cast<Connection*>(conn));
		return count;
	}

	bool Aurata::Transaction(std::function<bool()> f) {
		auto conn = reinterpret_cast<sqlite3*>(this->conn_manage.GetConnection());
		this->connections.push(reinterpret_cast<Connection*>(conn));
		this->Update("BEGIN TRANSACTION;", {});

		if (!f()) {
			this->Update("ROLLBACK;", {});
			this->conn_manage.FreeConnection(reinterpret_cast<Connection*>(conn));
			this->connections.pop();
			return false;
		}

		this->Update("COMMIT;", {});
		this->conn_manage.FreeConnection(reinterpret_cast<Connection*>(conn));
		this->connections.pop();
		return true;
	}
}