cmake_minimum_required(VERSION 3.16)
project(project02 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall -Wextra -std=c11 -D_GNU_SOURCE")

include_directories("C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v11.0\\include")
include_directories(include)
link_directories(include)

file(GLOB SOURCES src/*.c)
add_executable(project02 ${SOURCES})
