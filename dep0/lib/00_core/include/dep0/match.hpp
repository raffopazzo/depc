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

#include <cassert>
#include <concepts>
#include <variant>

namespace dep0 {

/** @cond DEP0_DOXYGEN_HIDE */
namespace detail::match {

template <typename T, typename F, typename... Fs>
requires (std::invocable<F, T> or sizeof...(Fs) > 0ul)
decltype(auto) invoke_one(T&& x, F&& f, Fs&&... fs)
{
    if constexpr (std::invocable<F, T>)
        return std::forward<F>(f)(std::forward<T>(x));
    else
        return invoke_one(std::forward<T>(x), std::forward<Fs>(fs)...);
}

template <std::size_t I, typename... Ts, typename... Fs>
decltype(auto) impl(std::integral_constant<std::size_t, I>, std::variant<Ts...> const& x, Fs&&... fs)
{
    auto const* const p = std::get_if<I>(&x);
    if constexpr (I+1 < sizeof...(Ts))
    {
        if (p)
            return invoke_one(*p, std::forward<Fs>(fs)...);
        else
            return impl(std::integral_constant<std::size_t, I+1>{}, x, std::forward<Fs>(fs)...);
    }
    else
    {
        assert(p and "corrupted variant uninhabited by any of its types");
        return invoke_one(*p, std::forward<Fs>(fs)...);
    }
}

template <std::size_t I, typename... Ts, typename... Fs>
decltype(auto) impl(std::integral_constant<std::size_t, I>, std::variant<Ts...>& x, Fs&&... fs)
{
    auto* const p = std::get_if<I>(&x);
    if constexpr (I+1 < sizeof...(Ts))
    {
        if (p)
            return invoke_one(*p, std::forward<Fs>(fs)...);
        else
            return impl(std::integral_constant<std::size_t, I+1>{}, x, std::forward<Fs>(fs)...);
    }
    else
    {
        assert(p and "corrupted variant uninhabited by any of its types");
        return invoke_one(*p, std::forward<Fs>(fs)...);
    }
}

} // namespace detail::match
/** @endcond */

/**
 * @brief Invokes the first function that can accept the value currently stored in the input variant.
 *
 * It is a compile-time error if any of the variant alternatives cannot be accepted by at least one function.
 *
 * If more than one function can accept the same variant alternative, only the first function will be invoked.
 *
 * @remarks This is **not** equivalent to calling `std::visit()` with an overloaded visitor function because
 * the function invoked is the first viable one, not the best candidate according to overload resolution rules.
 * Instead, this is equivalent to calling `std::visit()` with the result of `boost::hana::overload_linearly()`.
 *
 * @return The result returned by the invoked function.
 */
template <typename... Ts, typename... Fs>
decltype(auto) match(std::variant<Ts...> const& x, Fs&&... fs)
{
    return detail::match::impl(std::integral_constant<std::size_t, 0>{}, x, std::forward<Fs>(fs)...);
}

/**
 * @brief Overload taking a mutable-reference, thus allowing the invoked function to mutate the visited value.
 *
 * Except for this, every other detail is exactly the same as the overload taking a const-reference.
 */
template <typename... Ts, typename... Fs>
decltype(auto) match(std::variant<Ts...>& x, Fs&&... fs)
{
    return detail::match::impl(std::integral_constant<std::size_t, 0>{}, x, std::forward<Fs>(fs)...);
}

} // namespace dep0
