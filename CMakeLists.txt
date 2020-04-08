cmake_minimum_required(VERSION 3.10)
project(pa2 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic")
add_executable(pa2 main.c logger.h ipc.c io.h io.c pa2345.h bank_robbery.c local_state.c local_state.h child.c child.h parent.c parent.h logger.c)

find_library(libruntime libruntime.so paths ${PROJECT_SOURCE_DIR})
target_link_libraries(pa2 ${libruntime})