cmake_minimum_required(VERSION 3.10)

project(AsyncNetMsg)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)
if(BOOST_FOUND)
    message(STATUS "Boost version: ${Boost_VERSION}")
else()
    message(STATUS "Boost not found")
endif()

include_directories(../Common/Tools)

add_library(AsyncNetMsg SHARED 
                    ClientImpl.cpp 
                    AsioClient.cpp
                    ../Common/Tools/StringFunc.cc
                    )

target_link_libraries(AsyncNetMsg PRIVATE Boost::system)
target_link_directories(AsyncNetMsg PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_options(AsyncNetMsg PRIVATE -ggdb -pthread)
