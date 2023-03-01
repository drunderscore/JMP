/*
 * Copyright (c) 2023, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Shader.h"

namespace JMP::GL
{
class Program
{
public:
    Program() : m_program(glCreateProgram()) {}

    // Never copy this!
    Program(const Program&) = delete;

    // ...but moving it is okay :)
    Program(Program&& other) : m_program(other.program()) { other.m_program = 0; }

    ~Program()
    {
        if (m_program != 0)
            glDeleteProgram(m_program);
    }

    static Program link_vertex_and_fragment_shaders(VertexShader& vertex_shader, FragmentShader& fragment_shader)
    {
        Program program;

        program.attach_shader(vertex_shader);
        program.attach_shader(fragment_shader);
        program.link();

        return std::move(program);
    }

    template<GLenum TShaderType>
    void attach_shader(const Shader<TShaderType>& shader)
    {
        glAttachShader(m_program, shader.shader());
    }

    void link()
    {
        glLinkProgram(m_program);

        GLint success{};
        glGetProgramiv(program(), GL_LINK_STATUS, &success);

        // FIXME: Include info log string in exception
        if (!success)
            throw std::runtime_error("Failed to link program");
    }

    void use() { glUseProgram(m_program); }

    GLuint program() const { return m_program; }

private:
    explicit Program(GLuint program) : m_program(program) {}

    GLuint m_program{};
};
}