cmake_minimum_required(VERSION 3.10)
project(pa1 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic")

add_executable(pa1 main.c logger.h ipc.c io.h io.c)