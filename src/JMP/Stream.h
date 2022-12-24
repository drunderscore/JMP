/*
 * Copyright (c) 2022, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "ScopeGuard.h"
#include <span>
#include <stdexcept>
#include <vector>

namespace JMP
{
class Stream
{
public:
    enum class SeekOrigin
    {
        Start,
        Current,
        End
    };

    virtual std::vector<uint8_t> read(size_t number_of_bytes) = 0;
    virtual void seek(size_t offset, SeekOrigin) = 0;
    virtual size_t index() const = 0;

    template<typename Callback>
    auto temporarily_seek(size_t offset, SeekOrigin seek_origin, Callback callback)
    {
        ScopeGuard seek_to_initial_index{[this, initial_index = index()] { seek(initial_index, SeekOrigin::Start); }};
        seek(offset, seek_origin);
        return callback();
    }
};

class MemoryStream : public Stream
{
public:
    explicit MemoryStream(std::span<uint8_t> bytes) : m_bytes(bytes) {}

    std::vector<uint8_t> read(size_t number_of_bytes) override
    {
        if (m_index + number_of_bytes >= m_bytes.size())
            throw std::runtime_error("Cannot read past the end of the stream");

        std::vector<uint8_t> bytes(m_bytes.begin() + m_index, m_bytes.begin() + m_index + number_of_bytes);
        m_index += number_of_bytes;

        return std::move(bytes);
    }

    void seek(size_t offset, SeekOrigin seek_origin) override
    {
        ssize_t new_index{};

        switch (seek_origin)
        {
            case SeekOrigin::Start:
                new_index = offset;
                break;
            case SeekOrigin::Current:
                new_index = m_index + offset;
                break;
            case SeekOrigin::End:
                new_index = m_bytes.size() - offset;
                break;
        }

        if (new_index < 0)
            throw std::runtime_error("Cannot seek before beginning of the stream");

        if (new_index >= m_bytes.size())
            throw std::runtime_error("Cannot seek past end of the stream");

        m_index = new_index;
    }

    size_t index() const override { return m_index; }

private:
    size_t m_index{};
    std::span<uint8_t> m_bytes;
};
}