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

    ~FileStream();

    std::vector<uint8_t> read(size_t number_of_bytes) override;
    void write(std::span<uint8_t> bytes_to_write) override;
    void seek(size_t offset, SeekOrigin seek_origin) override;
    size_t index() const override;

private:
    explicit FileStream(FILE* file) : m_file(file) {}

    static int whence_for_seek_origin(SeekOrigin seek_origin);

    FILE* m_file{};
};
}