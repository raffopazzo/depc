/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

/**
 * Compute the hash code of the given expression.
 * Alpha-equivalent expressions are guaranteeed to have the same hash code.
 */
template <Properties P> std::size_t hash_code(expr_t<P> const&);

} // namespace dep0::ast

#include "dep0/ast/hash_code_impl.hpp"
