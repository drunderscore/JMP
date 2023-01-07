/*
 * Copyright (c) 2023, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "FileStream.h"
#include <cassert>
#include <stdexcept>

namespace JMP
{
FileStream::~FileStream()
{
    if (m_file)
    {
        fclose(m_file);
        m_file = nullptr;
    }
}

std::vector<uint8_t> FileStream::read(size_t number_of_bytes)
{
    std::vector<uint8_t> bytes;
    bytes.resize(number_of_bytes);

    if (fread(bytes.data(), 1, number_of_bytes, m_file) != number_of_bytes)
        throw std::runtime_error("Failed to fread for stream for all bytes requested");

    return std::move(bytes);
}

void FileStream::write(std::span<uint8_t> bytes_to_write)
{
    if (fwrite(bytes_to_write.data(), 1, bytes_to_write.size(), m_file) != bytes_to_write.size())
        throw std::runtime_error("Failed to fwrite for stream for all bytes requested");
}

void FileStream::seek(size_t offset, SeekOrigin seek_origin)
{
    if (fseek(m_file, offset, whence_for_seek_origin(seek_origin)) != 0)
        throw std::runtime_error("Failed to fseek for stream");
}

size_t FileStream::index() const
{
    auto tell = ftell(m_file);
    if (tell == -1)
        throw std::runtime_error("Failed to ftell for stream to get index");

    return tell;
}

int FileStream::whence_for_seek_origin(SeekOrigin seek_origin)
{
    switch (seek_origin)
    {
        case SeekOrigin::Start:
            return SEEK_SET;
        case SeekOrigin::Current:
            return SEEK_CUR;
        case SeekOrigin::End:
            return SEEK_END;
    }

    assert(false);
}
}