#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::transform {

void substitute(
    typecheck::expr_t::abs_t::arg_iterator begin,
    typecheck::expr_t::abs_t::arg_iterator end,
    typecheck::body_t&,
    typecheck::expr_t::var_t const&,
    typecheck::expr_t const&);
void substitute(typecheck::expr_t&, typecheck::expr_t::var_t const&, typecheck::expr_t const&);

} // namespace dep0::transform

