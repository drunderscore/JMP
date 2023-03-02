/*
 * Copyright (c) 2023, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "../ScopeGuard.h"
#include <glad/gl.h>

namespace JMP::GL
{
template<GLenum TTarget, GLenum TName>
class Texture
{
public:
    Texture() { glGenTextures(1, &m_name); }

    // Never copy this!
    Texture(const Texture&) = delete;

    // ...but moving it is okay :)
    Texture(Texture&& other) : m_name(other.name()) { other.m_name = 0; }

    ~Texture()
    {
        if (m_name != 0)
            glDeleteTextures(1, &m_name);
    }

    GLuint name() const { return m_name; }

    void bind() { glBindTexture(TTarget, m_name); }

    // NOTE: This check for GL_TEXTURE_2D is a bit incorrect, as glTexImage2D can be used with many other texture
    //       targets.
    static void set_data(GLint level, GLint internal_format, GLsizei width, GLsizei height, GLenum format, GLenum type,
                         const uint8_t* data)
        requires(TTarget == GL_TEXTURE_2D)
    {
        glTexImage2D(TTarget, level, internal_format, width, height, 0, format, type, data);
    }

    static void set_parameter(GLenum name, GLint value) { glTexParameteri(TTarget, name, value); }
    static void set_parameter(GLenum name, GLfloat value) { glTexParameterf(TTarget, name, value); }

    template<typename Callback>
    void with_bound(Callback callback)
    {
        GLint previously_bound_texture{};
        glGetIntegerv(TName, &previously_bound_texture);

        JMP::ScopeGuard bind_previously_bound_texture{
            [previously_bound_texture]() { glBindTexture(TTarget, previously_bound_texture); }};

        bind();
        callback();
    }

private:
    explicit Texture(GLuint name) : m_name(name) {}

    GLuint m_name{};
};

using Texture2D = Texture<GL_TEXTURE_2D, GL_TEXTURE_BINDING_2D>;
}