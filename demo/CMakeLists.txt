cmake_minimum_required (VERSION 3.5)

add_executable (demo "main.cpp" "ConnPoolManage.h" "ConnPoolManage.cpp" "FriendsService.hpp")

target_link_libraries(demo PUBLIC aurata)