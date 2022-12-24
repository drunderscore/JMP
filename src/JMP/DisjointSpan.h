/*
 * Copyright (c) 2022, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <span>
#include <stdexcept>
#include <vector>

namespace JMP
{
template<typename T>
class DisjointSpan
{
public:
    DisjointSpan() = default;
    DisjointSpan(std::initializer_list<std::span<T>> spans) { m_spans.insert(m_spans.end(), spans); }

    void push(std::span<T> span) { m_spans.push_back(span); }
    void reserve(size_t value) { m_spans.reserve(value); }

    T& at(size_t index) const
    {
        size_t offset{};

        for (auto span : m_spans)
        {
            if (span.empty())
                continue;

            if (auto next_offset = span.size() + offset; next_offset <= index)
            {
                offset = next_offset;
                continue;
            }

            return span[index - offset];
        }

        throw std::runtime_error("Unable to index DisjointSpan out of bounds");
    }

    const std::vector<std::span<T>>& spans() const { return m_spans; }
    std::vector<std::span<T>>& spans() { return m_spans; }

private:
    std::vector<std::span<T>> m_spans;
};
}