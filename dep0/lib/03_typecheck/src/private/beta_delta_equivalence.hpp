/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `dep0::typecheck::is_beta_delta_equivalent()`.
 * @see @ref beta_reduction
 */
#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

#include "dep0/error.hpp"

namespace dep0::typecheck {

/**
 * @brief Checks whether two types are equivalent under beta-delta conversion rules;
 * if they are not equivalent, it returns the reason why.
 * @see @ref beta_reduction
 */
dep0::expected<std::true_type>
is_beta_delta_equivalent(
    env_t const&,
    ctx_t const&,
    sort_t const&,
    sort_t const&);

} // namespace dep0::typecheck
