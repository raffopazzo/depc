/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include <map>
#include <optional>

namespace dep0::typecheck {

/**
 * Return the set of necessary substitutions that transform the 1st expression into the 2nd,
 * or nullopt if the two expressions cannot be unified.
 * For example, `unify(true_t(a < b), true_t(i < n + 1))` returns `{{a, i}, {b, n + 1}}`.
 */
std::optional<std::map<expr_t::var_t, expr_t>> unify(expr_t const&, expr_t const&);

} // namespace dep0::typecheck
