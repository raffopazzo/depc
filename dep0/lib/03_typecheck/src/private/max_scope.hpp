/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header file containing the declaration of `max_scope()`.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"

#include "dep0/error.hpp"

namespace dep0::typecheck {

/**
 * @brief Return the highest scope ID that appears in all subexpressions of the given expression
 * with respect to the given context.
 */
expected<std::size_t> max_scope(ctx_t const& ctx, expr_t const& expr);

} // namespace dep0::typecheck
