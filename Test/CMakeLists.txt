cmake_minimum_required(VERSION 3.5)

project(test_use_client)

set(CMAKE_BUILD_TYPE Debug)

add_executable(test_use_client main.cpp)
target_link_libraries(test_use_client dl)
target_compile_options(test_use_client PRIVATE -ggdb -pthread)
#target_link_libraries(test_use_clietn ../build/libGeneralClient.so)