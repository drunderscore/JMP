/*
 * Copyright (c) 2023, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Forward.h"
#include <concepts>
#include <cstdint>
#include <span>
#include <variant>
#include <vector>

namespace JMP
{
// A BitStream is so drastically different from a Stream because it deals on a bit-level, which is unlike most
// streamable types, so I've made it a completely separate that only acts on bytes. This does mean rehashing existing
// code/ideas from Stream, but linking those two together is very non-trivial, and has very little to gain.
//
// In the case that we want to have an automatically-allocating BitStream as you write to it, we also allow having an
// std::vector<uint8_t> backing that we own.
class BitStream
{
public:
    enum class SeekOrigin
    {
        Start,
        Current,
        End
    };

    explicit BitStream(std::span<uint8_t> bytes) : m_bytes(bytes) {}
    BitStream() : m_bytes(std::vector<uint8_t>{}) {}

    bool read();

    // FIXME: Support specifying endianness.
    // FIXME: Support doubles.
    template<typename T>
    T read(uint8_t number_of_bits = sizeof(T) * 8)
        requires(std::is_integral<T>::value || std::is_same<T, float>::value)
    {
        Integral<T> value{};

        for (auto i = 0; i < number_of_bits; i++)
        {
            if (read())
                value |= (1 << i);
        }

        return *reinterpret_cast<T*>(&value);
    }

    void write(bool value);

    // FIXME: Support specifying endianness.
    // FIXME: Support doubles.
    template<typename T>
    void write(T value, uint8_t number_of_bits = sizeof(T) * 8)
        requires(std::is_integral<T>::value || std::is_same<T, float>::value)
    {
        for (auto i = 0; i < number_of_bits; i++)
            write(((*reinterpret_cast<Integral<T>*>(&value)) & (1 << i)) != 0);
    }

    virtual size_t current_bit() const { return m_current_bit; }
    void seek(size_t offset, SeekOrigin seek_origin);

    inline std::span<uint8_t> bytes_span()
    {
        return std::visit(overloaded{[](std::span<uint8_t> bytes) { return bytes; },
                                     [](std::vector<uint8_t>& bytes) -> std::span<uint8_t> {
                                         return {bytes.data(), bytes.size()};
                                     }},
                          m_bytes);
    }

private:
    std::variant<std::span<uint8_t>, std::vector<uint8_t>> m_bytes;
    size_t m_current_bit{};

    template<typename T>
        requires(std::is_integral<T>::value || std::is_same<T, float>::value)
    using Integral = std::conditional<std::is_same<T, float>::value, uint32_t, T>::type;

    template<class... Ts>
    struct overloaded : Ts...
    {
        using Ts::operator()...;
    };
    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;
};
}