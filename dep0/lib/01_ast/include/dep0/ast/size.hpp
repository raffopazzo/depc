/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header that declares `dep0::ast::size()`.
 */
#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

/**
 * @brief Return the height of the given AST.
 * 
 * For example:
 *   - `x` has height 0
 *   - `x + y` and `f(x)` have height 1
 *   - `x + y + z` and `(x + y) + (x + z)` have height 2
 */
template <Properties P> std::size_t size(expr_t<P> const&);

} // namespace dep0::ast

#include "dep0/ast/size_impl.hpp"
