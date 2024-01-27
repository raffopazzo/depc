#pragma once

#include "dep0/typecheck/ast.hpp"

#include <vector>

namespace dep0::llvmgen {

bool is_first_order_function_type(std::vector<typecheck::func_arg_t> const&, typecheck::expr_t const& ret_type);
bool is_first_order_type(typecheck::expr_t const&);

} // namespace dep0::llvmgen

