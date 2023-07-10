#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"

#include <functional>
#include <ranges>
#include <utility>

namespace dep0::transform {

using transform_t = std::function<expected<std::true_type>(typecheck::module_t&)>;

template <std::ranges::range R>
// `requires transform_t`
expected<std::true_type> run(R r, typecheck::module_t& m)
{
    for (auto&& f: r)
        if (auto result = f(m); not result)
            return std::move(result.error());
    return std::true_type{};
}

} // namespace dep0::transform
