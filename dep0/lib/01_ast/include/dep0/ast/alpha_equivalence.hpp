/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Functions that test for @ref alpha_equivalence.
 */
#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/error.hpp"

namespace dep0::ast {

/**
 * @brief Tests whether two expressions are alpha-equivalent; if not, returns an error with the reason why.
 * @see @ref alpha_equivalence
 */
template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent(expr_t<P> const&, expr_t<P> const&);

} // namespace dep0::ast

#include "dep0/ast/alpha_equivalence_impl.hpp"
