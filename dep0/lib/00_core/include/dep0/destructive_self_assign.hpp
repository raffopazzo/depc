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
