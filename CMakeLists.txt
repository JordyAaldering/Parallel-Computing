cmake_minimum_required(VERSION 3.16)
project(parallel-computing C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -fopenmp")

add_executable(relax project/relax.c)
add_executable(relaxOptimized project/relaxOptimized.c)
add_executable(relaxOpenMP project/relaxOpenMP.c)
add_executable(relaxMPI project/relaxMPI.c)
