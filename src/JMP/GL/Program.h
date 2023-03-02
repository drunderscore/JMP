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
    Program() : m_name(glCreateProgram()) {}

    // Never copy this!
    Program(const Program&) = delete;

    // ...but moving it is okay :)
    Program(Program&& other) : m_name(other.name()) { other.m_name = 0; }

    ~Program()
    {
        if (m_name != 0)
            glDeleteProgram(m_name);
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
        glAttachShader(m_name, shader.name());
    }

    void link()
    {
        glLinkProgram(m_name);

        GLint success{};
        glGetProgramiv(name(), GL_LINK_STATUS, &success);

        // FIXME: Include info log string in exception
        if (!success)
            throw std::runtime_error("Failed to link program");
    }

    void use() { glUseProgram(m_name); }

    GLuint name() const { return m_name; }

private:
    explicit Program(GLuint name) : m_name(name) {}

    GLuint m_name{};
};
}