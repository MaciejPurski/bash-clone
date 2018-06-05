cmake_minimum_required(VERSION 3.9)
project(bash_clone)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIR})

add_executable(environmentTest
        tests/environment-tests.cpp Environment.h Environment.cpp BashExceptions.h)

add_executable(engineTest
        tests/engine-tests.cpp ExecutionEngine.h ExecutionEngine.cpp Interpreter.h Interpreter.cpp BashExceptions.h )

add_executable(interpreterTest
        tests/interpreter-tests.cpp Interpreter.h Interpreter.cpp BashExceptions.h Lexer.h Lexer.cpp Parser.h Parser.cpp Environment.h Environment.cpp Token.h )

add_executable(bash_clone
        main.cpp ExecutionEngine.cpp ExecutionEngine.h Interpreter.cpp Interpreter.h Command.h Environment.cpp Environment.h Shell.cpp Shell.h Job.cpp Job.h Token.h Lexer.h Lexer.cpp Parser.h Parser.cpp)

