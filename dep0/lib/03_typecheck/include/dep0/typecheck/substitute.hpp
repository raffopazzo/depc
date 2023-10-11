#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/scope_map.hpp"

namespace dep0::typecheck {

// Careful use of a substitution context allows to perform
// capture-avoiding substitution without explicitly renaming type variables.
using substitution_context_t = scope_map<source_text, typecheck::type_t>;

void substitute(typecheck::type_t&, substitution_context_t const&);
void substitute(typecheck::type_t&, typecheck::type_t::var_t const&, typecheck::type_t const&);
void substitute(typecheck::type_t&, source_text const& var_name, typecheck::type_t const&);

} // namespace dep0::typecheck
