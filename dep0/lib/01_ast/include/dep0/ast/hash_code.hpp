/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `dep0::ast::hash_code()`.
 */
#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

/**
 * @brief Compute the hash code of the given expression.
 * @remarks Alpha-equivalent expressions are guaranteeed to have the same hash code.
 * @see @ref alpha_equivalence
 */
template <Properties P> std::size_t hash_code(expr_t<P> const&);

} // namespace dep0::ast

#include "dep0/ast/hash_code_impl.hpp"

#include <functional>
namespace std {

template <dep0::ast::Properties P>
struct hash<dep0::ast::expr_t<P>>
{
    size_t operator()(dep0::ast::expr_t<P> const& x) const
    {
        return dep0::ast::hash_code(x);
    }
};

} // namespace std
