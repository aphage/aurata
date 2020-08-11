
cmake_minimum_required (VERSION 3.5)

add_library (aurata STATIC "Aurata.cpp" "Aurata.h" "json/json.h" "json/json-forwards.h" "jsoncpp.cpp" "sqlite/sqlite3.h" "sqlite/sqlite3ext.h" "sqlite/sqlite3.c" "Connection.h" "ConnectionPool.h" "SQLiteConnectionPool.h" "SQLiteConnectionPool.cpp"   "AurataParam.h")

