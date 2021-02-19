cmake_minimum_required(VERSION 3.17)
project(northern_tech C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-g -pedantic -Wall -fsanitize=address")
add_executable(northern_tech main.c)