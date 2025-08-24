/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Pattern-matching for `std::variant`.
 */
#pragma once

#include <array>
#include <type_traits>
#include <variant>

#include <boost/hana.hpp>

namespace dep0 {

/**
 * @brief Invokes the best candidate that can accept the value currently stored in the input variant.
 *
 * The invoked function is picked according to standard overload resolution rules.
 *
 * @return The result returned by the invoked function.
 */
template <typename... Ts, typename... Fs>
decltype(auto) match(std::variant<Ts...> const& x, Fs&&... fs)
{
    using F = decltype(boost::hana::overload(std::forward<Fs>(fs)...));
    using R = decltype(std::declval<F>()(std::get<0>(x)));
    auto const jump_table = [&] <std::size_t... Is> (std::index_sequence<Is...>)
    {
        static_assert((std::is_same_v<R, decltype(std::declval<F>()(std::get<Is>(x)))> or ...));
        return std::array{
            (+[] (std::variant<Ts...> const& x, F&& f) -> R
            {
                return std::forward<F>(f)(std::get<Is>(x));
            })...
        };
    }(std::make_index_sequence<sizeof...(Ts)>{});
    return jump_table[x.index()](x, boost::hana::overload(std::forward<Fs>(fs)...));
}

/**
 * @brief Overload taking a mutable-reference, thus allowing the invoked function to mutate the visited value.
 *
 * Except for this, every other detail is exactly the same as the overload taking a const-reference.
 */
template <typename... Ts, typename... Fs>
decltype(auto) match(std::variant<Ts...>& x, Fs&&... fs)
{
    using F = decltype(boost::hana::overload(std::forward<Fs>(fs)...));
    using R = decltype(std::declval<F>()(std::get<0>(x)));
    auto const jump_table = [&] <std::size_t... Is> (std::index_sequence<Is...>)
    {
        static_assert((std::is_same_v<R, decltype(std::declval<F>()(std::get<Is>(x)))> or ...));
        return std::array{
            (+[] (std::variant<Ts...>& x, F&& f) -> R
            {
                return std::forward<F>(f)(std::get<Is>(x));
            })...
        };
    }(std::make_index_sequence<sizeof...(Ts)>{});
    return jump_table[x.index()](x, boost::hana::overload(std::forward<Fs>(fs)...));
}

} // namespace dep0
