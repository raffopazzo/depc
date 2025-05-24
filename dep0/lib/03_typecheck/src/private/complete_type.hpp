/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/environment.hpp"

#include "dep0/error.hpp"

#include <type_traits>

namespace dep0::typecheck {

/**
 * @brief Checks whether a type is complete with respect to a given environment.
 *
 * The concept of a complete type is similar to C++ where a self-referential struct is incomplete,
 * unless the self-reference is inside some sort of box, like an array or a pointer.
 * For example, `struct t { t x; };` is an incomplete type but `struct t { array_t(t, 1) v; };` is complete.
 */
expected<std::true_type> is_complete_type(env_t const&, expr_t const&);

} // namespace dep0::typecheck
