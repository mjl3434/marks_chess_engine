cmake_minimum_required(VERSION 3.10)

project(marks_chess_engine VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# Find threading library
find_package(Threads REQUIRED)

# Add the source files from the src/ directory
file(GLOB_RECURSE SOURCES src/*.cpp)

# Specify the output directories
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
#set(CMAKE_OBJECT_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/obj)

# Define the main executable as a bunch of libraries linked together
add_library(marks_chess_engine_lib STATIC ${SOURCES})
add_executable(marks_chess_engine src/main.cpp)
target_link_libraries(marks_chess_engine marks_chess_engine_lib Threads::Threads)

add_subdirectory(test)