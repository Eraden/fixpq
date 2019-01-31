cmake_minimum_required(VERSION 3.13)
project(fixpq)

add_definitions(-Wall)
include_directories(include)

add_executable(fixpq src/main.c)

