cmake_minimum_required(VERSION 3.9)
project(bash_clone)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIR})

add_executable(environmentTest
        environment-tests.cpp Environment.h Environment.cpp)

add_executable(engineTest
        engine-tests.cpp Engine.h Engine.cpp Interpreter.h Interpreter.cpp)

add_executable(interpreterTest
        interpreter-tests.cpp Interpreter.h Interpreter.cpp)

add_executable(bash_clone
        main.cpp Engine.cpp Engine.h Interpreter.cpp Interpreter.h Command.h Environment.cpp Environment.h)
