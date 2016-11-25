set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(cache_sim cache_sim.cpp)

configure_file(examples/cacheconfig.txt ${CMAKE_BINARY_DIR}/cacheconfig.txt COPYONLY)
configure_file(examples/trace.txt ${CMAKE_BINARY_DIR}/trace.txt COPYONLY)
