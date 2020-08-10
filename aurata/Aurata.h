#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <deque>
#include <functional>

#include "json/json.h"
#include "AurataParam.h"
#include "Connection.h"
#include "ConnectionManage.h"

namespace aurata {

	class Aurata {
		public:
			Aurata();
			~Aurata();

			std::vector<Json::Value> Select(std::string&& sql, std::vector<AurataParam*>&& params = {});

			int Update(std::string&& sql, std::vector<AurataParam*>&& params = {});

			int Insert(std::string&& sql, std::vector<AurataParam*>&& params = {}, int64_t* auto_id = nullptr);

			bool Transaction(std::function<bool()> f);

		private:

			Connection* GetConnection(bool& is_local);
			void FreeConnection(bool& is_local, Connection* connection);

			static thread_local std::stack<Connection*> connections;
			ConnectionManage conn_manage;
	};

}
