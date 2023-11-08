/*
 * Copyright (c) 2023, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "BitStream.h"
#include "Stream.h"
#include <cstddef>
#include <stdexcept>

namespace JMP
{
bool BitStream::read()
{
    auto byte_index = m_current_bit >> 3;
    if (byte_index >= bytes_span().size())
        throw std::runtime_error("Cannot read past end of BitStream bytes");

    auto value = bytes_span().data()[byte_index] >> (m_current_bit & 7);
    m_current_bit++;
    return value & 1;
}

void BitStream::write(bool value)
{
    auto byte_index = m_current_bit >> 3;
    if (byte_index >= bytes_span().size())
    {
        std::visit(overloaded{[](std::span<uint8_t> bytes) {
                                  throw std::runtime_error("Cannot write past end of BitStream bytes");
                              },
                              [](std::vector<uint8_t>& bytes) { bytes.push_back({}); }},
                   m_bytes);
    }

    auto& byte = bytes_span()[byte_index];

    if (value)
        byte |= 1 << (m_current_bit & 7);
    else
        byte &= ~(1 << (m_current_bit & 7));

    m_current_bit++;
}

void BitStream::seek(size_t offset, SeekOrigin seek_origin)
{
    ssize_t new_current_bit{};

    switch (seek_origin)
    {
        case SeekOrigin::Start:
            new_current_bit = offset;
            break;
        case SeekOrigin::Current:
            new_current_bit = m_current_bit + offset;
            break;
        case SeekOrigin::End:
            new_current_bit = (bytes_span().size() * 8) - offset;
            break;
    }

    if (new_current_bit < 0)
        throw std::runtime_error("Cannot seek before beginning of the stream");

    if (new_current_bit >= bytes_span().size() * 8)
        throw std::runtime_error("Cannot seek past end of the stream");

    m_current_bit = new_current_bit;
}
}