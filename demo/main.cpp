
#include "../aurata/Aurata.h"
#include "../aurata/SQLiteConnectionPool.h"

using namespace std;
using namespace aurata;

int main() {
	auto conn_pool = new SQLiteConnectionPool("Hello-SQLite3.db", 2, 5);
	ConnectionManage conn_manage(conn_pool);

	Aurata aurata;

	auto list = aurata.Select("select * from friends where id = ? or id = ?", {
		new AurataParamInteger(1),
		new AurataParamInteger(2)
		});
	Json::StreamWriterBuilder builder;
	const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	for (auto&& v : list) {
		std::cout << "name: " << v["name"].asString() << std::endl;
	}

	int64_t id = 0;
	auto n = aurata.Insert("insert into friends(name,description) values(?, ?)", {
		new AurataParamText("Aurata"),
		new AurataParamText("Aurata is a fish!")
		}, &id);
	cout << "insert count : " << n << " id : " << id << endl;

	list = aurata.Select("select * from friends where id = ?", {
		new AurataParamInteger(id)
		});
	for (auto&& v : list) {
		auto s = v["name"].asString();
		std::cout << "name: " << s << std::endl;
	}
	aurata.Transaction([&] {
		aurata.Update("delete from friends");
		return false;
		});


	delete conn_pool;
	return 0;
}
