/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2022, James Puleo <james@jame.xyz>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <utility>

namespace JMP
{
template<typename Callback>
class ScopeGuard
{
public:
    ScopeGuard(Callback callback) : m_callback(std::move(callback)) {}

    ~ScopeGuard() { m_callback(); }

private:
    Callback m_callback;
};
}
