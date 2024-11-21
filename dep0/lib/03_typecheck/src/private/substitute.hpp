/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * Perform capture-avoiding substitution inside all arguments,
 * return type and (for lambda abstractions) the body of
 * either pi-types or lambda abstractions.
 *
 * @remarks In order to avoid accidental capture,
 * the function arguments inside the range [begin, end) may be modified.
 */
void substitute(
    expr_t::var_t const&,
    expr_t const&,
    std::vector<func_arg_t>::iterator begin,
    std::vector<func_arg_t>::iterator end,
    expr_t& ret_type,
    body_t* body);

} // namespace dep0::typecheck
