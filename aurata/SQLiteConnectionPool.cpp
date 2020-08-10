#include "SQLiteConnectionPool.h"
#include "sqlite/sqlite3.h"


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
		this->DestroyConnection(conn);
	}
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

void SQLiteConnectionPool::DestroyConnection(Connection* conn) {
	sqlite3_close(reinterpret_cast<sqlite3*>(conn));
}

Connection* SQLiteConnectionPool::GetConnection() {
	do {
		if (!this->connections.empty()) {
			std::lock_guard<std::mutex> lock(this->get_conn_lock);
			auto conn = this->connections.back();
			this->connections.pop_back();
			this->current_connection--;
			return conn;
		}
		if (this->current_connection < this->max_connections) {
			std::lock_guard<std::mutex> lock(this->get_conn_lock);
			this->current_connection++;
			return this->CreateConnection();
		}

		std::this_thread::yield();
	} while (true);
}

void SQLiteConnectionPool::FreeConnection(Connection* connection) {
	if (this->current_connection > this->max_connections) {
		this->DestroyConnection(connection);
		return;
	}
	std::lock_guard<std::mutex> lock(this->get_conn_lock);
	this->connections.push_back(connection);
	this->current_connection++;
}