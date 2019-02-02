cmake_minimum_required(VERSION 3.13)
project(fixpq)

include(GoogleTest)

find_package(CMocka REQUIRED)

add_definitions(-Wall -Wpedantic)

include_directories(include tests)

file(COPY ${CMAKE_SOURCE_DIR}/examples DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/tmp DESTINATION ${CMAKE_BINARY_DIR})

set(SOURCE src/simple.c src/parser.c src/lexer.c)
set(TEST_SOURCE tests/parser_test.c tests/lexer_test.c)

add_executable(fixpq ${SOURCE} src/main.c)
add_executable(tests ${TEST_SOURCE} ${SOURCE} tests/main.c)

target_link_libraries(fixpq ${GTest_LIBRARY})
target_link_libraries(tests cmocka)

enable_testing()

