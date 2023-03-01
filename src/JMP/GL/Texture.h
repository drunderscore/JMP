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
    class ModificationContext
    {
    public:
        friend class Texture;

        // NOTE: This check for GL_TEXTURE_2D is a bit incorrect, as glTexImage2D can be used with many other texture
        //       targets.
        void set_data(GLint level, GLint internal_format, GLsizei width, GLsizei height, GLenum format, GLenum type,
                      const uint8_t& data)
            requires(TTarget == GL_TEXTURE_2D)
        {
            glTexImage2D(TTarget, level, internal_format, width, height, 0, format, type, &data);
        }

        void set_parameter(GLenum name, GLint value) { glTexParameteri(TTarget, name, value); }
        void set_parameter(GLenum name, GLfloat value) { glTexParameterf(TTarget, name, value); }

    private:
        explicit ModificationContext(Texture& texture) : m_texture(texture) {}

        Texture& m_texture;
    };

    Texture() { glGenTextures(1, &m_texture); }

    // Never copy this!
    Texture(const Texture&) = delete;

    // ...but moving it is okay :)
    Texture(Texture&& other) : m_texture(other.texture()) { other.m_texture = 0; }

    ~Texture()
    {
        if (m_texture != 0)
            glDeleteTextures(1, &m_texture);
    }

    GLuint texture() const { return m_texture; }

    void bind() { glBindTexture(TTarget, m_texture); }

    template<typename Callback>
    void with_bound(Callback callback)
    {
        GLint previously_bound_texture{};
        glGetIntegerv(TName, &previously_bound_texture);

        JMP::ScopeGuard bind_previously_bound_texture{
            [previously_bound_texture]() { glBindTexture(TTarget, previously_bound_texture); }};

        bind();
        callback(create_modification_context());
    }

private:
    explicit Texture(GLuint texture) : m_texture(texture) {}

    ModificationContext create_modification_context() { return ModificationContext(*this); }

    GLuint m_texture{};
};

using Texture2D = Texture<GL_TEXTURE_2D, GL_TEXTURE_BINDING_2D>;
}