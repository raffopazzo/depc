#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

type_t::var_t rename(
    type_t::var_t const&,
    type_t::arr_t::arg_iterator begin,
    type_t::arr_t::arg_iterator end,
    type_t& ret_type);

} // namespace dep0::typecheck
