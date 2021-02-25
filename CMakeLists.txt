cmake_minimum_required(VERSION 3.17)
project(minimalist_shell C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-g -pedantic -Wall")
add_executable(
		minimalist_shell
		main.c
		builtins.c
		interpret.c
		stringlist.c
		stringlist.h
		run.c
		run.h
)