/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `dep0::ast::unwrap_because()`.
 */
#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

/**
 * @brief If the given expression has the form `value because reason` return a reference to the value,
 * recursively until a non-because expression is found.
 */
template <Properties P>
expr_t<P> const& unwrap_because(expr_t<P> const& expr)
{
    auto p = &expr;
    while (auto q = std::get_if<typename expr_t<P>::because_t>(&p->value))
        p = &q->value.get();
    return *p;
}

} // namespace dep0::ast
