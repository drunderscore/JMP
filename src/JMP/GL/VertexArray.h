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
class VertexArray
{
public:
    VertexArray() { glGenVertexArrays(1, &m_name); }

    // Never copy this!
    VertexArray(const VertexArray&) = delete;

    // ...but moving it is okay :)
    VertexArray(VertexArray&& other) : m_name(other.name()) { other.m_name = 0; }

    ~VertexArray()
    {
        if (m_name != 0)
            glDeleteVertexArrays(1, &m_name);
    }

    GLuint name() const { return m_name; }

    void bind() { glBindVertexArray(m_name); }

    template<typename Callback>
    void with_bound(Callback callback)
    {
        GLint previously_bound_vertex_array{};
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previously_bound_vertex_array);

        JMP::ScopeGuard bind_previously_bound_vertex_array{
            [previously_bound_vertex_array]() { glBindVertexArray(previously_bound_vertex_array); }};

        bind();
        callback();
    }

private:
    explicit VertexArray(GLuint name) : m_name(name) {}

    GLuint m_name{};
};
}