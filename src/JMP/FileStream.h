/*
 * Copyright (c) 2022, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Stream.h"
#include <cstdio>

namespace JMP
{
class FileStream : public Stream
{
public:
    // Instead of calling the constructor, make people call this function, to explicitly indicate that we will close the
    // file ourself.
    static FileStream adopt(FILE* file) { return std::move(FileStream(file)); }

    FileStream(FileStream&& other)
    {
        m_file = other.m_file;
        other.m_file = nullptr;
    }

    ~FileStream()
    {
        if (m_file)
        {
            fclose(m_file);
            m_file = nullptr;
        }
    }

    std::vector<uint8_t> read(size_t number_of_bytes) override
    {
        std::vector<uint8_t> bytes;
        bytes.resize(number_of_bytes);

        if (fread(bytes.data(), 1, number_of_bytes, m_file) != number_of_bytes)
            throw std::runtime_error("Failed to fread for stream for all bytes requested");

        return std::move(bytes);
    }

    void seek(size_t offset, SeekOrigin seek_origin) override
    {
        if (fseek(m_file, offset, whence_for_seek_origin(seek_origin)) != 0)
            throw std::runtime_error("Failed to fseek for stream");
    }

    size_t index() const override
    {
        auto tell = ftell(m_file);
        if (tell == -1)
            throw std::runtime_error("Failed to ftell for stream to get index");

        return tell;
    }

private:
    explicit FileStream(FILE* file) : m_file(file) {}

    static int whence_for_seek_origin(SeekOrigin seek_origin)
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
    }

    FILE* m_file{};
};
}