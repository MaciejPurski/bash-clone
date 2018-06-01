cmake_minimum_required(VERSION 3.9)
project(bash_clone)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIR})

add_executable(environmentTest
        tests/environment-tests.cpp Environment.h Environment.cpp)

add_executable(engineTest
        tests/engine-tests.cpp ExecutionEngine.h ExecutionEngine.cpp Interpreter.h Interpreter.cpp)

add_executable(interpreterTest
        tests/interpreter-tests.cpp Interpreter.h Interpreter.cpp)

add_executable(bash_clone
        main.cpp ExecutionEngine.cpp ExecutionEngine.h Interpreter.cpp Interpreter.h Command.h Environment.cpp Environment.h Terminal.cpp Terminal.h)
