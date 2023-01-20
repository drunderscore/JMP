cmake_minimum_required(VERSION 3.16)
project(JMP CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(JMP
        src/JMP/BitStream.cpp
        src/JMP/FileStream.cpp
        src/JMP/Signature.cpp
        )

if (WIN32)
    target_sources(JMP PRIVATE src/JMP/Platforms/Windows.cpp)
elseif (UNIX)
    target_sources(JMP PRIVATE src/JMP/Platforms/Linux.cpp)
else ()
    message(FATAL_ERROR "Missing platform implementation")
endif ()