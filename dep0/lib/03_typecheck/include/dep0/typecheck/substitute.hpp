#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/scope_map.hpp"

namespace dep0::typecheck {

using substitution_context_t = scope_map<type_t::var_t, type_t>;

void substitute(type_t&, substitution_context_t const&);

} // namespace dep0::typecheck
