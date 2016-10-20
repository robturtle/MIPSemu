cmake_minimum_required(VERSION 3.6)
project(MIPSemu)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("include")

add_executable(Ins_test examples/Ins_test.cpp)
add_executable(Memory_test examples/Memory_test.cpp)
