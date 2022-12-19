cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect x86
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "^(i.86|x86|x86_64|amd64|AMD64)$")
    message(STATUS "x86* CPU detected")
    set(CPU_IS_x86 TRUE)
endif()

file(GLOB_RECURSE libldpc_cs src/*.cpp)

add_library(ldpc SHARED ${libldpc_cs})
target_include_directories(ldpc PUBLIC src)

if(CPU_IS_x86)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1 -mavx2")
    target_compile_definitions(ldpc PUBLIC LIBLDPC_ENABLE_SSE=1)
    target_compile_definitions(ldpc PUBLIC LIBLDPC_ENABLE_AVX=1)
else()
    target_compile_definitions(ldpc PUBLIC LIBLDPC_ENABLE_SSE=0)
    target_compile_definitions(ldpc PUBLIC LIBLDPC_ENABLE_AVX=0)
endif()

# ############################################
add_executable(ldpc_decode decode.cpp)
target_include_directories(ldpc_decode PUBLIC src)
target_link_libraries(ldpc_decode PUBLIC ldpc)

add_executable(ldpc_benchmark benchmark.cpp)
target_include_directories(ldpc_benchmark PUBLIC src)
target_link_libraries(ldpc_benchmark PUBLIC ldpc)