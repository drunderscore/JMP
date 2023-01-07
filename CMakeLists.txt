cmake_minimum_required(VERSION 3.16)
project(JMP CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(JMP
        src/JMP/FileStream.cpp
        )