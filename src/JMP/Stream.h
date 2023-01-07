/*
 * Copyright (c) 2022-2023, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "ScopeGuard.h"
#include <cstdint>
#include <span>
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
    virtual void write(std::span<uint8_t> bytes_to_write) = 0;
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
}