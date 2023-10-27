#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

void substitute(type_t&, type_t::var_t const&, type_t const&);
void substitute(
    type_t::arr_t::arg_iterator begin,
    type_t::arr_t::arg_iterator end,
    type_t& ret_type,
    type_t::var_t const&,
    type_t const&);

} // namespace dep0::typecheck
