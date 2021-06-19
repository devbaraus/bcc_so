cmake_minimum_required(VERSION 3.16)
project(produtor_consumidor C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS -pthread)

include_directories(.)

add_executable(produtor_consumidor
        main.c)

