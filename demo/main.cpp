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

#include <iostream>
#include <string>
#include <vector>

#include "../aurata/Aurata.h"
#include "../aurata/SQLiteConnectionPool.h"

#include "FriendsService.hpp"

using namespace std;
using namespace aurata;

int main() {
#ifdef _MSC_VER
	system("chcp 65001");
#endif

	Json::StreamWriterBuilder builder;
	const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

	auto conn_pool = new SQLiteConnectionPool("Hello-SQLite3.db", 2, 5);
	ConnPoolManage conn_pool_manage(conn_pool);
	Aurata aur(conn_pool);

	auto r = aur.Transaction([&] {
		aur.Update("drop table if exists friends");
		aur.Update("CREATE TABLE friends( \
			id integer PRIMARY key, \
			name varchar(255) not null, \
			description varchar(255) \
		); ");
		return true;
	});
	
	cout << "is ok : " << r << endl;


	FriendsService friends_service;

	Json::Value friend1;
	friend1["name"] = u8"御坂美琴";
	friend1["description"] = u8"The electric sparkle glittering on your fingertips is my rock solid faith for life!";

	friends_service.Add(std::move(friend1));

	Json::Value friend2;
	friend2["name"] = u8"DIYgod";
	friend2["description"] = u8"写代码写到世界充满爱💕";

	friends_service.Add(std::move(friend2));

	Json::Value friend3;
	friend3["name"] = u8"林果";
	friend3["description"] = u8"咕噜灵波~";

	friends_service.Add(std::move(friend3));

	Json::Value friend4;
	friend4["name"] = u8"himself65";
	friend4["description"] = u8"我好失败啊 ( 我太成功了.jpg";

	friends_service.Add(std::move(friend4));

	Json::Value friend5;
	friend5["name"] = u8"迟先生";
	friend5["description"] = u8"我好失败啊 ( 我太成功了.jpg";

	friends_service.Add(std::move(friend5));

	std::vector<Json::Value> friends;

	for (int i = 0; i < 10777; i++) {
		friends.push_back(friend1);
	}

	friends_service.Add(std::move(friends));

	Json::Value search_friend;
	search_friend["name"] = u8"御坂";

	auto list = friends_service.List(std::move(search_friend), 1, 10);
	cout << "list count : " << list.size() << endl;
	for (auto& v : list) {
		cout << "===================================================" << endl;
		cout << "id : " << v["id"].asInt64() << endl;
		cout << "name : " << v["name"].asString() << endl;
		cout << "description : " << v["description"].asString() << endl;
		cout << "===================================================" << endl;
	}

	cout << endl;

	delete conn_pool;
	return 0;
}
