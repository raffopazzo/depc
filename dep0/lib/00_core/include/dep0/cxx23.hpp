/**
 * @file cxx23.hpp
 * @brief Poor man version of C++23 standard library functions which can be used until the compiler is upgraded.
 */

#pragma once

#include <utility>

namespace dep0::cxx23::ranges {

template <typename R, typename T, typename F>
T fold_left(R&& range, T init, F&& f)
{
    for (auto const& x: std::forward<R>(range))
        init = f(init, x);
    return init;
}

template <template <typename...> typename C, typename R>
auto to(R&& range)
{
    using value_type = std::remove_cvref_t<decltype(*std::begin(range))>;
    C<value_type> c;
    static_assert(requires{ c.push_back(std::declval<value_type>()); });
    for (auto&& x: std::forward<R>(range))
        // TODO support other container types
        c.push_back(std::forward<value_type>(x));
    return c;
}

} // namespace dep0::cxx23::ranges
