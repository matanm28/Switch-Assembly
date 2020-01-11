cmake_minimum_required(VERSION 3.15)
project(ex3 C)

set(CMAKE_C_STANDARD 99)

add_executable(ex3 compile.c switch.c CRLF_switch.c test.c)