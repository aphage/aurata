#pragma once

#include "Connection.h"

class ConnectionPool {
public:
	virtual Connection* GetConnection() = 0;
	virtual void FreeConnection(Connection* connection) = 0;
};