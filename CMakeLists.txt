cmake_minimum_required(VERSION 3.9)
project(bash_clone)

set(CMAKE_CXX_STANDARD 14)

add_executable(bash_clone
        main.cpp Engine.cpp Engine.h Interpreter.cpp Interpreter.h Command.h Environment.cpp Environment.h)
