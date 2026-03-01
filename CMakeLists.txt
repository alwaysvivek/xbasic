cmake_minimum_required(VERSION 4.1)
project(simplelang_compiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(simplelang_compiler main.cpp
        Token.h
        Lexer.cpp
        Lexer.h)
