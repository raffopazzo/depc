#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/error.hpp"
#include <utility>

namespace dep0::transform {

/**
 * LLVM expects all basic blocks to terminate with a branch instruction.
 * A function whose return type is `unit_t` may not contain a return statement.
 * So this pass adds a return statement at the end, if not already there.
 */
expected<std::true_type> reify_return_unit(typecheck::module_t&);

} // namespace dep0::transform
