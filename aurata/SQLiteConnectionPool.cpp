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

#include <thread>

#include "SQLiteConnectionPool.h"
#include "sqlite/sqlite3.h"

namespace aurata {

	SQLiteConnectionPool::SQLiteConnectionPool(std::string db_name, int initial_connections, int max_connections) {
		this->db_name = db_name;
		this->initial_connections = initial_connections;
		this->current_connection = this->initial_connections;
		this->max_connections = max_connections;

		for (int i = 0; i < this->initial_connections; i++) {
			this->connections.push_back(this->CreateConnection());
		}
	}

	SQLiteConnectionPool::~SQLiteConnectionPool() {
		for (auto&& conn : this->connections) {
			this->FreeConnection(conn);
		}
		this->initial_connections = 0;
		this->current_connection = 0;
	}

	Connection* SQLiteConnectionPool::CreateConnection() {
		sqlite3* conn;
		auto rc = sqlite3_open(this->db_name.c_str(), &conn);
		if (rc != SQLITE_OK) {
			throw new std::runtime_error("sqlite3_open error!");
		}
		return reinterpret_cast<Connection*>(conn);
	}

	void SQLiteConnectionPool::FreeConnection(Connection* conn) {
		sqlite3_close(reinterpret_cast<sqlite3*>(conn));
	}

	Connection* SQLiteConnectionPool::GetConnection() {
		do {
			if (!this->connections.empty()) {
				std::lock_guard<std::mutex> lock(this->get_conn_lock);
				if (!this->connections.empty()) {
					auto conn = this->connections.back();
					this->connections.pop_back();
					this->current_connection--;
					return conn;
				}
			}
			if (this->current_connection < this->max_connections) {
				std::lock_guard<std::mutex> lock(this->get_conn_lock);
				if (this->current_connection < this->max_connections) {
					this->current_connection++;
					return this->CreateConnection();
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			std::this_thread::yield();
		} while (true);
	}

	void SQLiteConnectionPool::ReturnConnection(Connection* connection) {
		std::lock_guard<std::mutex> lock(this->get_conn_lock);
		if (this->current_connection > this->initial_connections) {
			this->FreeConnection(connection);
			return;
		}
		this->connections.push_back(connection);
		this->current_connection--;
	}
}
