cmake_minimum_required(VERSION 4.1)
project(attempt_0 C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

add_executable(attempt_0 main.c wutils.h wutils.c)

find_package(raylib CONFIG REQUIRED)
target_link_libraries(attempt_0 PRIVATE raylib)
