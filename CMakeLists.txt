cmake_minimum_required(VERSION 3.17)
project(minimalist_shell C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-g -pedantic -Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(
		minimalist_shell
		src/main.c
		src/builtins.c
		src/interpret.c
		src/stringlist.c
		src/stringlist.h
		src/run.c
		src/run.h
)