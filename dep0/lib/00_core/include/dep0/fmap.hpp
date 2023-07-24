#pragma once

#include "dep0/error.hpp"

#include <type_traits>
#include <vector>

namespace dep0 {

template <typename T, typename F>
auto fmap(std::vector<T> const& xs, F const& f)
-> std::vector<std::invoke_result_t<F const&, typename std::vector<T>::value_type>>
{
    std::vector<std::invoke_result_t<F const&, typename std::vector<T>::value_type>> result;
    result.reserve(xs.size());
    for (auto const& x: xs)
        result.push_back(f(x));
    return result;
}

template <typename T, typename F>
// requires ...
auto fmap_or_error(std::vector<T> const& xs, F const& f)
-> expected<
    std::vector<typename std::invoke_result_t<F const&, typename std::vector<T>::value_type>::value_type>,
    typename std::invoke_result_t<F const&, typename std::vector<T>::value_type>::error_type
    >
{
    using result_t =
        expected<
            std::vector<typename std::invoke_result_t<F const&, typename std::vector<T>::value_type>::value_type>,
            typename std::invoke_result_t<F const&, typename std::vector<T>::value_type>::error_type
        >;
    result_t result;
    result.value().reserve(xs.size());
    for (auto const& x: xs)
        if (auto r = f(x))
            result.value().push_back(std::move(*r));
        else
            return std::move(r.error());
    return result;
}


} // namespace dep0
