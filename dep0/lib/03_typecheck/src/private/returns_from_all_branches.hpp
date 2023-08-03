#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

bool returns_from_all_branches(body_t const&);
bool returns_from_all_branches(stmt_t::if_else_t const&);

} // namespace dep0::typecheck
