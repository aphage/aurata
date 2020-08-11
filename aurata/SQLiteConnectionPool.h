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

#pragma once

#include <iostream>
#include <vector>
#include <mutex>

#include "ConnectionPool.h"

namespace aurata {

	class SQLiteConnectionPool : public ConnectionPool {
	public:
		SQLiteConnectionPool(std::string db_name, int initial_connections, int max_connections);
		~SQLiteConnectionPool();


		virtual Connection* GetConnection();
		virtual void ReturnConnection(Connection* connection);
	private:
		std::string db_name;
		int initial_connections;
		int current_connection;
		int max_connections;
		std::vector<Connection*> connections;
		std::mutex get_conn_lock;


		Connection* CreateConnection();
		void FreeConnection(Connection* conn);
	};
}
