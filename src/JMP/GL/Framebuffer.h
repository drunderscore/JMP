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
    Framebuffer() { glGenFramebuffers(1, &m_name); }

    // Never copy this!
    Framebuffer(const Framebuffer&) = delete;

    // ...but moving it is okay :)
    Framebuffer(Framebuffer&& other) : m_name(other.name()) { other.m_name = 0; }

    ~Framebuffer()
    {
        if (m_name != 0)
            glDeleteFramebuffers(1, &m_name);
    }

    GLuint name() const { return m_name; }

    void bind(GLenum target = GL_FRAMEBUFFER) { glBindFramebuffer(target, m_name); }

    template<GLenum TTextureName>
    static void attach_texture(GLenum attachment, const Texture<GL_TEXTURE_2D, TTextureName>& texture, GLint level,
                               GLenum target = GL_FRAMEBUFFER)
    {
        attach_texture(attachment, GL_TEXTURE_2D, texture.name(), level, target);
    }

    static void attach_texture(GLenum attachment, GLenum texture_target, GLuint texture, GLint level,
                               GLenum target = GL_FRAMEBUFFER)
    {
        glFramebufferTexture2D(target, attachment, texture_target, texture, level);
    }
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
        callback();
    }

private:
    explicit Framebuffer(GLuint name) : m_name(name) {}

    GLuint m_name{};
};
}