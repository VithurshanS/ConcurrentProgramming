cmake_minimum_required(VERSION 3.28.3)
project(MutualExclusion C)

set(CMAKE_C_STANDARD 23)

add_executable(MutualExclusion main.c
        critical_section_problem.c
        critical_section_problem.h
        bounded_buffer_problem.c
        bounded_buffer_problem.h)
