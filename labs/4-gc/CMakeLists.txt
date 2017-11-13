cmake_minimum_required(VERSION 3.6)
project(mirungc)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES mas.c)
add_executable(mirun_gc ${SOURCE_FILES})
