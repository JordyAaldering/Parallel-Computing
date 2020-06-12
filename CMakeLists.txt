cmake_minimum_required(VERSION 3.16)
project(parallel-computing C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -fopenmp")

include_directories("C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v11.0\\include")
add_executable(relax project02/relax.c project02/simple.c)
