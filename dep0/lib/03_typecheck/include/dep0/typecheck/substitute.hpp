#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

void substitute(typecheck::type_t&, typecheck::type_t::var_t const&, typecheck::type_t const&);
void substitute(typecheck::type_t&, source_text const& var_name, typecheck::type_t const&);

} // namespace dep0::typecheck
