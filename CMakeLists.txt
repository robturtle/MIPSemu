cmake_minimum_required(VERSION 3.6)
project(MIPSemu)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("include")

#configure_file(${CMAKE_SOURCE_DIR}/imem.txt ${CMAKE_BINARY_DIR}/imem.txt COPYONLY)
