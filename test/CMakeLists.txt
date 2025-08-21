# CMakeLists.txt in test/
cmake_minimum_required(VERSION 3.10)
project(ChessEngineTests)

# Add the main engine source directory
include_directories(${CMAKE_SOURCE_DIR}/src)

# List of test files
set(TEST_SOURCES
    Application-test.cpp
    ChessEngine-test.cpp
    ChessGame-test.cpp
    Common-test.cpp
    GameState-test.cpp
    HelperFunctions-test.cpp
    Rules-test.cpp
    UCICommand-test.cpp
    run_all_tests.cpp
)

# Add each test as an executable
add_executable(run_all_tests ${TEST_SOURCES})
target_link_libraries(run_all_tests marks_chess_engine_lib Threads::Threads)