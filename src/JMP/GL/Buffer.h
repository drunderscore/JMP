/*
 * Copyright (c) 2023, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "../ScopeGuard.h"
#include <glad/gl.h>
#include <span>

namespace JMP::GL
{
template<GLenum TTarget, GLenum TName>
class Buffer
{
public:
    Buffer() { glGenBuffers(1, &m_buffer); }

    // Never copy this!
    Buffer(const Buffer&) = delete;

    // ...but moving it is okay :)
    Buffer(Buffer&& other) : m_buffer(other.buffer()) { other.m_buffer = 0; }

    ~Buffer()
    {
        if (m_buffer != 0)
            glDeleteBuffers(1, &m_buffer);
    }

    GLuint buffer() const { return m_buffer; }

    void bind() { glBindBuffer(TTarget, m_buffer); }

    template<typename T>
    static void set_data(std::span<T> bytes, GLenum usage)
    {
        glBufferData(TTarget, bytes.size() * sizeof(T), bytes.data(), usage);
    }

    template<typename Callback>
    void with_bound(Callback callback)
    {
        GLint previously_bound_buffer{};
        glGetIntegerv(TName, &previously_bound_buffer);

        JMP::ScopeGuard bind_previously_bound_buffer{
            [previously_bound_buffer]() { glBindBuffer(TTarget, previously_bound_buffer); }};

        bind();
        callback();
    }

private:
    explicit Buffer(GLuint buffer) : m_buffer(buffer) {}

    GLuint m_buffer{};
};

using ArrayBuffer = Buffer<GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING>;
using ElementArrayBuffer = Buffer<GL_ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER_BINDING>;
}