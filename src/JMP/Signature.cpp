/*
 * Copyright (c) 2023, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Signature.h"
#include <cctype>
#include <charconv>

namespace JMP
{
Signature::Signature(std::string_view signature)
{
    for (auto i = 0; i < signature.length(); i++)
    {
        auto c = signature[i];

        // Spaces are insignificant
        if (isspace(c))
            continue;

        if (c == '?')
        {
            m_values.push_back({});
            continue;
        }

        uint8_t expected_byte_value;
        std::from_chars(signature.data() + i, signature.data() + i + 2, expected_byte_value, 16);
        m_values.push_back(expected_byte_value);
        i++;
    }
}

void* Signature::find_in(std::span<uint8_t> bytes)
{
    for (auto i = 0; i < bytes.size() - m_values.size(); i++)
    {
        bool failed = false;

        for (auto j = 0; j < m_values.size(); j++)
        {
            auto& value = m_values[j];
            if (!value.has_value())
                continue;

            if (*(bytes.data() + i + j) != *value)
            {
                failed = true;
                break;
            }
        }

        if (!failed)
            return static_cast<void*>(bytes.data() + i);
    }

    return nullptr;
}
}