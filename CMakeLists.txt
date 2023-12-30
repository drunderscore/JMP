cmake_minimum_required(VERSION 3.16)
project(JMP CXX C)

set(CMAKE_CXX_STANDARD 20)

option(JMP_OPENGL "Compile with OpenGL support" OFF)

add_library(JMP
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

if (${JMP_OPENGL})
    target_sources(JMP PRIVATE src/GLAD/src/gl.c)
    target_include_directories(JMP PUBLIC src/GLAD/include)
endif ()

set_target_properties(JMP PROPERTIES POSITION_INDEPENDENT_CODE ON)
