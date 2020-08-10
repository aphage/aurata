
#include "ConnectionManage.h"

ConnectionPool* ConnectionManage::conn_pool = nullptr;
ConnectionManage::ConnectionManage(ConnectionPool* conn_pool) {
	this->conn_pool = conn_pool;
}

ConnectionManage::ConnectionManage() {}
ConnectionManage::~ConnectionManage() {}

Connection* ConnectionManage::GetConnection() {
	return this->conn_pool->GetConnection();
}

void ConnectionManage::FreeConnection(Connection* conn) {
	this->conn_pool->FreeConnection(conn);
}