#pragma once

#include "dep0/typecheck/ast.hpp"

#include <string>
#include <vector>

namespace dep0::llvmgen {

std::string make_mangled_name(source_text const&, std::vector<typecheck::type_t> const&);

} // namespace dep0::llvmgen
