#pragma once

#include "dep0/typecheck/ast.hpp"

#include <vector>

namespace dep0::llvmgen {

/**
 * Checks whether a function type formed by the given arguments and return type is of the 1st order.
 * A function type is of the 1st order if both its argument types and the return type are all 1st order types.
 *
 * @param args
 *      The arguments forming the function types; only their types matter for this purpose.
 *
 * @param ret_type
 *      The return type of the function type.
 *
 * @return True if the function type formed by the given arguments and return type is of the 1st order.
 */
bool is_first_order_function_type(std::vector<typecheck::func_arg_t> const&, typecheck::expr_t const& ret_type);

/**
 * Checks whether a type is a 1st order type or, in simple words, "a C type".
 * For example: primitives, 1st order function types and (possibly multidimensional) arrays on elements of 1st order.
 * In contrast, higher order types are parametric types, dependent types, etc.
 *
 * @return True if the given type expression is a 1st order type; false if it has higher order.
 */
bool is_first_order_type(typecheck::expr_t const&);

} // namespace dep0::llvmgen
