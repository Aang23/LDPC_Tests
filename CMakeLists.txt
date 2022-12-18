cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1 -mavx2")

file(GLOB_RECURSE SatDump_testing_CPPS src/*.cpp)

add_executable(ldpc_test ${SatDump_testing_CPPS})
target_include_directories(ldpc_test PUBLIC src)

#target_link_libraries(ldpc_test PUBLIC )