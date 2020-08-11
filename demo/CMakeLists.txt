cmake_minimum_required (VERSION 3.5)

add_executable (demo "main.cpp" "ConnPoolManage.h" "ConnPoolManage.cpp" "FriendsService.hpp")

target_link_libraries(demo PUBLIC aurata)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
target_link_libraries(demo PUBLIC
dl
pthread
stdc++
)
endif()