#pragma once

#include "ConnectionPool.h"

class ConnectionManage {
public:
	ConnectionManage(ConnectionPool* conn_pool);
	ConnectionManage();
	virtual ~ConnectionManage();

	virtual Connection* GetConnection();
	virtual void FreeConnection(Connection* conn);
private:
	static ConnectionPool* conn_pool;
};