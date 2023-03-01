/*
 * Copyright (c) 2023, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

// FIXME: It's a bit unfortunate this header is bloated with all these includes...
#include <filesystem>
#include <fstream>
#include <glad/gl.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace JMP::GL
{
template<GLenum TShaderType>
class Shader
{
public:
    using Self = Shader<TShaderType>;

    // Never copy this!
    Shader(const Self&) = delete;

    // ...but moving it is okay :)
    Shader(Self&& other) : m_shader(other.shader()) { other.m_shader = 0; }

    ~Shader()
    {
        if (m_shader != 0)
            glDeleteShader(m_shader);
    }

    GLuint shader() const { return m_shader; }

    static Self compile(std::string_view source_code)
    {
        Self shader(glCreateShader(TShaderType));

        // We need pointers to both of these (yes, const char**), because glShaderSource can take multiple at once.
        auto source_code_string = source_code.data();
        auto source_code_length = static_cast<GLint>(source_code.length());

        glShaderSource(shader.shader(), 1, &source_code_string, &source_code_length);
        glCompileShader(shader.shader());

        GLint success{};
        glGetShaderiv(shader.shader(), GL_COMPILE_STATUS, &success);

        // FIXME: Include info log string in exception
        if (!success)
            throw std::runtime_error("Failed to compile shader");

        return std::move(shader);
    }

    static Self compile_from_file(const std::filesystem::path& file_path)
    {
        // FIXME: Is this the C++ STL way to read files? Could this be improved?
        std::ifstream file_stream(file_path);

        if (file_stream.fail())
            throw std::runtime_error("Failed to read shader file");

        std::stringstream file_string_stream;
        file_string_stream << file_stream.rdbuf();

        if (file_stream.fail())
            throw std::runtime_error("Failed to read shader file");

        return compile(file_string_stream.view());
    }

private:
    explicit Shader(GLuint shader) : m_shader(shader) {}

    GLuint m_shader{};
};

using VertexShader = Shader<GL_VERTEX_SHADER>;
using FragmentShader = Shader<GL_FRAGMENT_SHADER>;
}