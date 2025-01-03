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

dep0::expected<std::true_type>
is_beta_delta_equivalent(
    env_t const& env,
    ctx_t const& ctx,
    sort_t const& x,
    sort_t const& y)
{
    struct visitor
    {
        env_t const& env;
        ctx_t const& ctx;

        dep0::expected<std::true_type> operator()(expr_t const& x, expr_t const& y) const
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
                if (beta_delta_normalize(env, ctx, x2) | beta_delta_normalize(env, ctx, y2)) // don't short-circuit
                    eq = is_alpha_equivalent(x2, y2);
            }
            return eq;
        }
        dep0::expected<std::true_type> operator()(kind_t const&, kind_t const&) const
        {
            return {};
        }
        dep0::expected<std::true_type> operator()(kind_t const& x, expr_t const& y)
        {
            std::ostringstream err;
            pretty_print(pretty_print(err << '`', x) << "` is not beta-delta-equivalent to `", y) << '`';
            return dep0::error_t(err.str());
        }
        dep0::expected<std::true_type> operator()(expr_t const& x, kind_t const& y)
        {
            std::ostringstream err;
            pretty_print(pretty_print(err << '`', x) << "` is not beta-delta-equivalent to `", y) << '`';
            return dep0::error_t(err.str());
        }
    };
    return std::visit(visitor{env, ctx}, x, y);
}

} // namespace dep0::typecheck
