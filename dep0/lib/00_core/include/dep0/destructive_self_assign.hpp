/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include <utility>

namespace dep0 {

/**
 * Assign `y` to `x` even though `y` is stored inside `x` or somehow kept alive by it.
 * Use this whenever a naive assignment `x=y` would result in undefined behaviour due to self-assignment.
 */
template <typename T>
void destructive_self_assign(T& x, T&& y)
{
    auto tmp = std::move(y);
    x = std::move(tmp);
}

} // namespace dep0
