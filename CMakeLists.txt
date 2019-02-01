cmake_minimum_required(VERSION 3.13)
project(fixpq)

add_definitions(-Wall)
include_directories(include)

set(SOURCE src/simple.c src/parser.c src/lexer.c src/main.c)

add_executable(fixpq ${SOURCE})

