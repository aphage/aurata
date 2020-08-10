#pragma once

#include <iostream>
#include <vector>
#include <mutex>

#include "ConnectionPool.h"

class SQLiteConnectionPool : public ConnectionPool {
public:
	SQLiteConnectionPool(std::string db_name, int initial_connections, int max_connections);
	~SQLiteConnectionPool();


	virtual Connection* GetConnection();
	virtual void FreeConnection(Connection* connection);
private:
	std::string db_name;
	int initial_connections;
	int current_connection;
	int max_connections;
	std::vector<Connection*> connections;
	std::mutex get_conn_lock;


	Connection* CreateConnection();
	void DestroyConnection(Connection* conn);
};