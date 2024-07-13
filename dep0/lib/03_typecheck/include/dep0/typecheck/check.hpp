#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/environment.hpp"
#include "dep0/typecheck/error.hpp"

#include "dep0/parser/ast.hpp"

namespace dep0::typecheck {

/**
 * Run type-checking on the given module inside the given enviornment.
 * There are two major use cases for this function:
 *   1. type-checking a user module
 *   2. type-checking the prelude module.
 * When type-checking the prelude, the enviornment is completely empty,
 * otherwise the enviornment should have the prelude pre-imported.
 */
expected<module_t> check(env_t const&, parser::module_t const&) noexcept;

} // namespace dep0::typecheck
