# cmake_minimum_required(VERSION <specify CMake version here>)
project(soma_vetores C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS -pthread)

add_executable(soma_vetores main.c)