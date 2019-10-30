cmake_minimum_required (VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)

add_executable (CollisionGen "CollisionGen.cpp" "CollisionGen.h")
target_link_libraries(CollisionGen ${CMAKE_CURRENT_SOURCE_DIR}/../lib/jsoncpp.lib)
target_include_directories(CollisionGen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)