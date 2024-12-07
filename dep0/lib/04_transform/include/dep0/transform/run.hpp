/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::transform::run()` and associated overloads.
 */
#pragma once

#include "dep0/transform/transform.hpp"

#include <optional>
#include <ranges>

namespace dep0::transform {

/**
 * @brief Applies all transformations from the given range to the given module.
 * @return Either success or the first failure if any transformations fails.
 */
template <std::ranges::range R>
expected<std::true_type> run(typecheck::module_t& m, R&& r) noexcept
requires (std::is_same_v<transform_t, std::ranges::range_value_t<R>>)
{
    for (auto&& f: std::forward<R>(r))
        if (auto result = f(m); not result)
            return result;
    return std::true_type{};
}

/**
 * @brief Applies all given transformations to the given module.
 * @return Either success or the first failure if any transformations fails.
 */
template <Transform... F>
expected<std::true_type> run(typecheck::module_t& m, F&&... f) noexcept
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
