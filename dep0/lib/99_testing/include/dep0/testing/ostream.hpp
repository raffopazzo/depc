#pragma once

#include "dep0/ast/ast.hpp"

#include <iostream>

namespace dep0::ast {

std::ostream& operator<<(std::ostream&, is_mutable_t);

} // namespace dep0::ast
