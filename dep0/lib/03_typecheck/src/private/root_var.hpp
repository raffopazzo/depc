/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring all overloads of `dep0::typecheck::root_var()`.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * @brief If the given member-access is rooted in a variable, return the address of such variable; nullptr otherwise.
 *
 * The root of a member-access is the top level variable, for example `x.y.z` is rooted in `x`,
 * whereas `f().y.z` is not rooted in a variable.
 */
expr_t::var_t const* root_var(expr_t::member_t const&);

/**
 * @brief If the given subscript-access is rooted in a variable, return the address of such variable; nullptr otherwise.
 *
 * The root of a member-access is the top level variable, for example `x[y]` is rooted in `x`,
 * whereas `f()[y]` is not rooted in a variable.
 */
expr_t::var_t const* root_var(expr_t::subscript_t const&);

} // namespace dep0::typecheck
