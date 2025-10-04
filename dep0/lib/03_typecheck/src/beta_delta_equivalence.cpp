/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/beta_delta_equivalence.hpp"

#include "dep0/typecheck/beta_delta_reduction.hpp"

#include "dep0/ast/alpha_equivalence.hpp"
#include "dep0/ast/pretty_print.hpp"

#include <sstream>

namespace dep0::typecheck {

template <typename T, typename U>
dep0::expected<std::true_type> not_equivalent(T const& x, U const& y)
{
    std::ostringstream err;
    pretty_print(pretty_print(err << '`', x) << "` is not beta-delta-equivalent to `", y) << '`';
    return dep0::error_t(err.str());
}

dep0::expected<std::true_type>
is_beta_delta_equivalent(sort_t const& x, sort_t const& y)
{
    struct visitor
    {
        dep0::expected<std::true_type> operator()(expr_t const& x, expr_t const& y) const
        {
            return is_beta_delta_equivalent(x, y);
        }
        dep0::expected<std::true_type> operator()(kind_t const&, kind_t const&) const
        {
            return {};
        }
        dep0::expected<std::true_type> operator()(kind_t const& x, expr_t const& y)
        {
            return not_equivalent(x, y);
        }
        dep0::expected<std::true_type> operator()(expr_t const& x, kind_t const& y)
        {
            return not_equivalent(x, y);
        }
    };
    return std::visit(visitor{}, x, y);
}

dep0::expected<std::true_type>
is_beta_delta_equivalent(expr_t const& x, sort_t const& y)
{
    if (auto const p = std::get_if<expr_t>(&y))
        return is_beta_delta_equivalent(x, *p);
    else
        return not_equivalent(x, y);
}

dep0::expected<std::true_type>
is_beta_delta_equivalent(sort_t const& x, expr_t const& y)
{
    if (auto const p = std::get_if<expr_t>(&x))
        return is_beta_delta_equivalent(*p, y);
    else
        return not_equivalent(x, y);
}

dep0::expected<std::true_type>
is_beta_delta_equivalent(expr_t const& x, expr_t const& y)
{
    // Try without normalizing first, under the assumptions that:
    // 1. often type expressions are already in normal-form;
    // 2. if not, alpha-equivalence fails immediately because of structural differences, eg `id(i32_t) vs i32_t`
    // But, admittedly, this has not been benchmarked yet.
    auto eq = is_alpha_equivalent(x, y);
    if (not eq)
    {
        auto x2 = x;
        auto y2 = y;
        if (beta_delta_normalize(x2) | beta_delta_normalize(y2)) // don't short-circuit
            eq = is_alpha_equivalent(x2, y2);
    }
    return eq;
}

} // namespace dep0::typecheck
