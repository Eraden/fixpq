cmake_minimum_required(VERSION 3.13)
project(fixpq)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})

find_package(CMocka REQUIRED)
find_package(Sanitizers REQUIRED)

option(USE_CLANG "build application with clang" ON) # OFF is the default

set(LLVM_ENABLE_PIC ON)
set(LLVM_USE_SANITIZER MemoryWithOrigins)
set(LLVM_ENABLE_LLD ON)
set(LLVM_PARALLEL_COMPILE_JOBS 4)
set(LLVM_PARALLEL_LINK_JOBS 4)

include_directories(include tests)

file(COPY ${CMAKE_SOURCE_DIR}/examples DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/tmp DESTINATION ${CMAKE_BINARY_DIR})

set(SOURCE src/simple.c src/parser.c src/lexer.c)
set(TEST_SOURCE tests/parser_test.c tests/lexer_test.c)

add_executable(fixpq ${SOURCE} src/main.c)
add_executable(tests ${TEST_SOURCE} ${SOURCE} tests/main.c)

target_link_libraries(fixpq)
target_link_libraries(tests cmocka)

add_sanitizers(fixpq)

enable_testing()
