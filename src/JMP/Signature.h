/*
 * Copyright (c) 2023, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

namespace JMP
{
class Signature
{
public:
    explicit Signature(std::string_view signature);

    void* find_in(std::span<uint8_t> bytes);

    const std::vector<std::optional<uint8_t>>& values() const { return m_values; }

private:
    std::vector<std::optional<uint8_t>> m_values;
};
}