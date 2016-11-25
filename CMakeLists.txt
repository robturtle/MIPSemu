cmake_minimum_required(VERSION 3.6)
project(MIPSemu)

include(CMakeToolsHelpers OPTIONAL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories("include")

configure_file(examples/imem.txt ${CMAKE_BINARY_DIR}/imem.txt COPYONLY)
configure_file(examples/dmem.txt ${CMAKE_BINARY_DIR}/dmem.txt COPYONLY)

add_executable(demo_Storage examples/demo_Storage.cpp)

#add_subdirectory(gigo)
add_subdirectory(lab2)
