cmake_minimum_required(VERSION 3.13)
project(MiniJVM C)

set(CMAKE_C_STANDARD 99)

include_directories(src)

add_executable(MiniJVM src/main.c src/loader/classloader.c src/loader/classloader.h src/util/debug.h src/lang/class.c src/lang/class.h src/lang/internal/const_pool.h src/lang/class_member.c src/lang/class_member.h src/lang/internal/flags.h)