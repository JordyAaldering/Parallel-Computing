cmake_minimum_required(VERSION 3.16)
project(parallel-computing C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -fopenmp")

add_executable(relax src/relax.c)
add_executable(relaxOptimized src/relaxOptimized.c)
add_executable(relaxOpenMP src/relaxOpenMP.c)
add_executable(relaxMPI src/relaxMPI.c)
