cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1 -mavx2")

file(GLOB_RECURSE libldpc_cs src/*.cpp)

add_library(ldpc SHARED ${libldpc_cs})
target_include_directories(ldpc PUBLIC src)

add_executable(ldpc_decode decode.cpp)
target_include_directories(ldpc_decode PUBLIC src)
target_link_libraries(ldpc_decode PUBLIC ldpc)

add_executable(ldpc_benchmark benchmark.cpp)
target_include_directories(ldpc_benchmark PUBLIC src)
target_link_libraries(ldpc_benchmark PUBLIC ldpc)