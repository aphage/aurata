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

#include <vector>

#include "../aurata/Aurata.h"
#include "../aurata/AurataParam.h"
#include "../aurata/json/json.h"

#include "ConnPoolManage.h"


class FriendsService {
public:
	FriendsService():aur(ConnPoolManage().GetConnPool()) {

	}

	std::vector<Json::Value> List(Json::Value&& v, int page, int page_size) {
		static std::string sql = "select * from friends where name like ?";
		if (page < 1) {
			page = 1;
		}
		if (page_size < 10) {
			page_size = 10;
		}

		if (!v.isMember("name") || !v["name"].isString()) {
			return std::move(std::vector<Json::Value>());
		}
		auto name = "%" + v["name"].asString() + "%";

		std::string s = sql + " limit " + std::to_string(page_size) + " offset " + std::to_string(--page * page_size);

		auto friends = aur.Select(std::move(s), {
			new aurata::AurataParamText(name)
		});

		return std::move(friends);
	}

	bool Add(Json::Value&& v) {
		static std::string sql = "insert into friends(name, description) values(?, ?)";
		if (!v.isMember("name") || !v["name"].isString()) {
			return false;
		}
		if (!v.isMember("description") || !v["description"].isString()) {
			return false;
		}
		auto name = v["name"].asString();
		auto description = v["description"].asString();
		auto id = aur.Insert(std::move(sql), {
			new aurata::AurataParamText(name),
			new aurata::AurataParamText(description)
		});
		return id > 0;
	}

	bool Add(std::vector<Json::Value>&& list) {
		if (list.empty()) {
			return false;
		}
		return aur.Transaction([&] {
			for (auto& v : list) {
				Add(std::move(v));
			}
			return true;
		});
	}

	bool Edit(Json::Value&& v) {
		static std::string sql = "update friends set name = ?, description = ? where id = ?";
		if (!v.isMember("id") || !v["id"].isInt()) {
			return false;
		}
		if (!v.isMember("name") || !v["name"].isString()) {
			return false;
		}
		if (!v.isMember("description") || !v["description"].isString()) {
			return false;
		}
		auto name = v["name"].asString();
		auto description = v["description"].asString();
		auto id = v["name"].asInt();

		auto count = aur.Update(std::move(sql), {
			new aurata::AurataParamText(name),
			new aurata::AurataParamText(description),
			new aurata::AurataParamInteger(id)
		});
		return count > 0;
	}

	bool Delete(int id) {
		static std::string sql = "delete from friends where id = ?";

		auto count = aur.Update(std::move(sql), {
			new aurata::AurataParamInteger(id)
		});
		return count > 0;
	}

	bool Delete(std::vector<int> list) {
		if (list.empty()) {
			return false;
		}
		return aur.Transaction([&] {
			for (auto& v : list) {
				Delete(v);
			}
			return true;
		});
	}
private:
	aurata::Aurata aur;
};
