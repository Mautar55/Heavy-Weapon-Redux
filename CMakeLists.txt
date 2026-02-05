cmake_minimum_required(VERSION 4.1)
project(attempt_0 C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

add_executable(attempt_0 main.c wutils.h wutils.c)
add_executable(test test.c wutils.h wutils.c)
target_link_libraries(test PRIVATE raylib)

find_package(raylib CONFIG REQUIRED)
target_link_libraries(attempt_0 PRIVATE raylib)
