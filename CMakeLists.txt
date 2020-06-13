cmake_minimum_required(VERSION 3.16)
project(parallel-computing C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -std=c11 -D_GNU_SOURCE") # -Wall -Wextra

include_directories("C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v11.0\\include")
add_executable(project02 project02/relax.c)

link_directories(${OpenCL_LIBRARY})
include_directories(${OpenCL_INCLUDE_DIRS})

set(LIBS ${LIBS} ${OpenCL_LIBRARY})
target_link_libraries(project02 ${LIBS})
