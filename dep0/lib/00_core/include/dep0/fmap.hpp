#pragma once

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

} // namespace dep0
