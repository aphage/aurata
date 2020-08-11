# aurata

I don't know anything, i'm just a fish

## Windows
Use Visual studio , please
```

## Linux

``` bash
mkdir out && cd out
cmake .. -DCMAKE_TOOLCHAIN_FILE=../linux.cmake
./demo/demo
```

``` c++
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
friend4["description"] = u8"我好失败啊 ( 我是垃圾.jpg";

friends_service.Add(std::move(friend4));

Json::Value friend5;
friend5["name"] = u8"立党";
friend5["description"] = u8"我好失败啊 ( 我是垃圾.jpg";

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
```

``` c++
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
```

``` text

is ok : 1
list count : 10
===================================================
id : 1
name : 御坂美琴
description : The electric sparkle glittering on your fingertips is my rock solid faith for life!
===================================================
===================================================
id : 6
name : 御坂美琴
description : The electric sparkle glittering on your fingertips is my rock solid faith for life!
===================================================
===================================================
id : 7
name : 御坂美琴
description : The electric sparkle glittering on your fingertips is my rock solid faith for life!
===================================================
===================================================
id : 8
name : 御坂美琴
description : The electric sparkle glittering on your fingertips is my rock solid faith for life!
===================================================
===================================================
id : 9
name : 御坂美琴
description : The electric sparkle glittering on your fingertips is my rock solid faith for life!
===================================================
===================================================
id : 10
name : 御坂美琴
description : The electric sparkle glittering on your fingertips is my rock solid faith for life!
===================================================
===================================================
id : 11
name : 御坂美琴
description : The electric sparkle glittering on your fingertips is my rock solid faith for life!
===================================================
===================================================
id : 12
name : 御坂美琴
description : The electric sparkle glittering on your fingertips is my rock solid faith for life!
===================================================
===================================================
id : 13
name : 御坂美琴
description : The electric sparkle glittering on your fingertips is my rock solid faith for life!
===================================================
===================================================
id : 14
name : 御坂美琴
description : The electric sparkle glittering on your fingertips is my rock solid faith for life!
===================================================

```