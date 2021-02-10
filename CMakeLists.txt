cmake_minimum_required(VERSION 3.13)
project(MiniJVM C)

set(CMAKE_C_STANDARD 99)

add_executable(MiniJVM src/main.c src/loader/classloader.c src/loader/classloader.h)