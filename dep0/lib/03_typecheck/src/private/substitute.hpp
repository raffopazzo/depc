#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * Perform capture-avoiding substitution inside all arguments,
 * return type and (for lambda abstractions) the body of
 * either pi-types or lambda abstractions.
 */
void substitute(
    expr_t::var_t const&,
    expr_t const&,
    std::vector<func_arg_t>::iterator begin,
    std::vector<func_arg_t>::iterator end,
    expr_t& ret_type,
    body_t* body);

} // namespace dep0::typecheck
