/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/environment.hpp"

#include "dep0/parser/ast.hpp"

#include "dep0/error.hpp"

namespace dep0::typecheck {

/**
 * Run type-checking on the given module inside the given enviornment.
 * There are two major use cases for this function:
 *   1. type-checking a user module
 *   2. type-checking the prelude module.
 * The input environment should be:
 *   - completely empty (i.e. default-constructed) when type-checking the prelude module
 *   - or the base environment (i.e. with prelude pre-imported) when type-checking a user module.
 */
expected<module_t> check(env_t const&, parser::module_t const&) noexcept;

} // namespace dep0::typecheck
