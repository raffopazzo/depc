#pragma once

#include "dep0/transform/transform.hpp"

#include <optional>
#include <ranges>

namespace dep0::transform {

template <std::ranges::range R>
requires (std::is_same_v<transform_t, std::ranges::range_value_t<R>>)
expected<std::true_type> run(typecheck::module_t& m, R&& r)
{
    for (auto&& f: std::forward<R>(r))
        if (auto result = f(m); not result)
            return result;
    return std::true_type{};
}

template <Transform... F>
expected<std::true_type> run(typecheck::module_t& m, F&&... f)
{
    std::optional<error_t> err;
    auto run_one = [&err, &m] <Transform F_> (F_&& f)
    {
        auto result = std::forward<F_>(f)(m);
        if (not result)
            err.emplace(std::move(result.error()));
        return result.has_value();
    };
    if ((run_one(std::forward<F>(f)) and ...))
        return std::true_type{};
    else
        return std::move(err.value());
}

} // namespace dep0::transform
