/*
 * Copyright (c) 2022, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Stream.h"

namespace JMP
{
class Reader
{
public:
    explicit Reader(Stream& stream) : m_stream(stream) {}

    template<typename T>
        requires(std::is_integral<T>::value || std::is_floating_point<T>::value)
    T read()
    {
        return *reinterpret_cast<T*>(m_stream.read(sizeof(T)).data());
    }

    // FIXME: Constrain this with concepts
    template<typename T>
    T read()
    {
        return T::read(*this);
    }

    template<typename T, typename Callback>
    std::vector<T> read_while(Callback callback)
    {
        std::vector<T> items;

        while (true)
        {
            auto item = read<T>();
            if (!callback(item))
                break;

            items.push_back(std::move(item));
        }

        return std::move(items);
    }

    Stream& stream() const { return m_stream; }

private:
    Stream& m_stream;
};
}