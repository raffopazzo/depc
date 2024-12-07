/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Functions that perform @ref beta_delta_reduction.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

namespace dep0::typecheck {

/**
 * @brief Perform a combined beta-delta normalization inside the given module.
 * @remarks Delta-unfolding is only performed inside a direct application,
 * eg `f(x)` for some global function `f`, but not everywhere, eg `return f`.
 * The reason for this is that we are interested in delta-unfolding only as
 * a way to extend beta normalization as far as possible.
 */
bool beta_delta_normalize(module_t&);

/**
 * @brief Perform a combined beta-delta normalization inside the given expression.
 * @remarks Delta-unfolding is only performed inside a direct application,
 * eg `f(x)` for some global function `f`, but not everywhere, eg `return f`.
 * The reason for this is that we are interested in delta-unfolding only as
 * a way to extend beta normalization as far as possible.
 */
bool beta_delta_normalize(env_t const&, ctx_t const&, expr_t&);

} // namespace dep0::typecheck
