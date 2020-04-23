cmake_minimum_required(VERSION 3.16)
project(parallel-computing C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -fopenmp")

file(GLOB ASSIGNMENTS src/assignment*.h)
add_executable(main src/main.c ${ASSIGNMENTS})
