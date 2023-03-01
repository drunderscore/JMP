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
    VertexArray() { glGenVertexArrays(1, &m_vertex_array); }

    // Never copy this!
    VertexArray(const VertexArray&) = delete;

    // ...but moving it is okay :)
    VertexArray(VertexArray&& other) : m_vertex_array(other.vertex_array()) { other.m_vertex_array = 0; }

    ~VertexArray()
    {
        if (m_vertex_array != 0)
            glDeleteVertexArrays(1, &m_vertex_array);
    }

    GLuint vertex_array() const { return m_vertex_array; }

    void bind() { glBindVertexArray(m_vertex_array); }

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
    explicit VertexArray(GLuint vertex_array) : m_vertex_array(vertex_array) {}

    GLuint m_vertex_array{};
};
}