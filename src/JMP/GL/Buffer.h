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
    class ModificationContext
    {
    public:
        friend class Buffer;

        template<typename T>
        void set_data(std::span<T> bytes, GLenum usage)
        {
            glBufferData(TTarget, bytes.size() * sizeof(T), bytes.data(), usage);
        }

    private:
        explicit ModificationContext(Buffer& buffer) : m_buffer(buffer) {}

        Buffer& m_buffer;
    };

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

    template<typename Callback>
    void with_bound(Callback callback)
    {
        GLint previously_bound_buffer{};
        glGetIntegerv(TName, &previously_bound_buffer);

        JMP::ScopeGuard bind_previously_bound_buffer{
            [previously_bound_buffer]() { glBindBuffer(TTarget, previously_bound_buffer); }};

        bind();
        callback(create_modification_context());
    }

private:
    explicit Buffer(GLuint buffer) : m_buffer(buffer) {}

    ModificationContext create_modification_context() { return ModificationContext(*this); }

    GLuint m_buffer{};
};

using ArrayBuffer = Buffer<GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING>;
}