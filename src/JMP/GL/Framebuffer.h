/*
 * Copyright (c) 2023, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "../ScopeGuard.h"
#include "Texture.h"
#include <glad/gl.h>

namespace JMP::GL
{
class Framebuffer
{
public:
    class ModificationContext
    {
    public:
        friend class Framebuffer;

        template<GLenum TTextureName>
        void attach_texture(GLenum attachment, const Texture<GL_TEXTURE_2D, TTextureName>& texture, GLint level,
                            GLenum target = GL_FRAMEBUFFER)
        {
            attach_texture(attachment, GL_TEXTURE_2D, texture.texture(), level, target);
        }

        void attach_texture(GLenum attachment, GLenum texture_target, GLuint texture, GLint level,
                            GLenum target = GL_FRAMEBUFFER)
        {
            glFramebufferTexture2D(target, attachment, texture_target, texture, level);
        }

    private:
        explicit ModificationContext(Framebuffer& framebuffer) : m_framebuffer(framebuffer) {}

        Framebuffer& m_framebuffer;
    };

    Framebuffer() { glGenFramebuffers(1, &m_framebuffer); }

    // Never copy this!
    Framebuffer(const Framebuffer&) = delete;

    // ...but moving it is okay :)
    Framebuffer(Framebuffer&& other) : m_framebuffer(other.framebuffer()) { other.m_framebuffer = 0; }

    ~Framebuffer()
    {
        if (m_framebuffer != 0)
            glDeleteFramebuffers(1, &m_framebuffer);
    }

    GLuint framebuffer() const { return m_framebuffer; }

    void bind(GLenum target = GL_FRAMEBUFFER) { glBindFramebuffer(target, m_framebuffer); }

    ModificationContext create_modification_context() { return ModificationContext(*this); }

    template<typename Callback>
    void with_bound(Callback callback, GLenum target = GL_FRAMEBUFFER)
    {
        GLint previously_bound_draw_framebuffer{};
        GLint previously_bound_read_framebuffer{};
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previously_bound_draw_framebuffer);
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &previously_bound_read_framebuffer);

        JMP::ScopeGuard bind_previously_bound_framebuffers{
            [previously_bound_draw_framebuffer, previously_bound_read_framebuffer]() {
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, previously_bound_draw_framebuffer);
                glBindFramebuffer(GL_READ_FRAMEBUFFER, previously_bound_read_framebuffer);
            }};

        bind(target);
        callback(create_modification_context());
    }

private:
    explicit Framebuffer(GLuint framebuffer) : m_framebuffer(framebuffer) {}

    GLuint m_framebuffer{};
};
}