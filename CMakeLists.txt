cmake_minimum_required(VERSION 3.9)
project(bash_clone)

set(CMAKE_CXX_STANDARD 11)

add_executable(bash_clone
        main.cpp ExecutionEngine.cpp ExecutionEngine.h Interpreter.cpp Interpreter.h Command.h Environment.cpp Environment.h Terminal.cpp Terminal.h)
