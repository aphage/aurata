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
#include <stack>
#include <deque>
#include <functional>

#include "json/json.h"
#include "sqlite/sqlite3.h"

#include "AurataParam.h"
#include "ConnectionPool.h"

namespace aurata {

	class Aurata {
		public:
			Aurata(ConnectionPool* conn_pool);
			~Aurata();

			std::vector<Json::Value> Select(std::string&& sql, std::vector<AurataParam*>&& params = {});

			int Update(std::string&& sql, std::vector<AurataParam*>&& params = {});

			int64_t Insert(std::string&& sql, std::vector<AurataParam*>&& params = {});

			bool Transaction(std::function<bool()> f);

			bool BackupDB(std::string&& file_name);

		private:
			Connection* GetConnection(bool& is_local);

			void ReturnConnection(Connection* connection, bool& is_local);

			void BindParams(sqlite3_stmt* stmt, std::vector<AurataParam*>&& params);

			void FreeParams(std::vector<AurataParam*>&& params);

			Json::Value ReadRow(sqlite3_stmt* stmt);

			ConnectionPool* conn_pool;
	};

}
