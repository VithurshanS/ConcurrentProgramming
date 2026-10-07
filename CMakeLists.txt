cmake_minimum_required(VERSION 3.28.3)
project(MutualExclusion C)

set(CMAKE_C_STANDARD 23)

add_executable(MutualExclusion main.c
        racecond.c
        racecond.h)
