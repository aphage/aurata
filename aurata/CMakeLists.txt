# CMakeList.txt: aurata 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 将源代码添加到此项目的可执行文件。
add_library (aurata STATIC "Aurata.cpp" "Aurata.h" "json/json.h" "json/json-forwards.h" "jsoncpp.cpp" "sqlite/sqlite3.h" "sqlite/sqlite3ext.h" "sqlite/sqlite3.c" "Connection.h" "ConnectionPool.h" "SQLiteConnectionPool.h" "SQLiteConnectionPool.cpp" "ConnectionManage.h" "ConnectionManage.cpp" "AurataParam.h")

# TODO: 如有需要，请添加测试并安装目标。
