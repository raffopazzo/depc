#pragma once

#include <concepts>
#include <variant>

namespace dep0 {

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
        return invoke_one(*p, std::forward<Fs>(fs)...);
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
        return invoke_one(*p, std::forward<Fs>(fs)...);
}

} // namespace detail::match

template <typename... Ts, typename... Fs>
decltype(auto) match(std::variant<Ts...> const& x, Fs&&... fs)
{
    return detail::match::impl(std::integral_constant<std::size_t, 0>{}, x, std::forward<Fs>(fs)...);
}

template <typename... Ts, typename... Fs>
decltype(auto) match(std::variant<Ts...>& x, Fs&&... fs)
{
    return detail::match::impl(std::integral_constant<std::size_t, 0>{}, x, std::forward<Fs>(fs)...);
}

} // namespace dep0
