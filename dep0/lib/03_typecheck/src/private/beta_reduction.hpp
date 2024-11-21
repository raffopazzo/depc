/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

namespace dep0::typecheck {

/**
 * @return true if beta normalization modified the given body.
 */
bool beta_normalize(body_t&);

/**
 * @return true if beta normalization modified the given expression.
 */
bool beta_normalize(expr_t&);

} // namespace dep0::typecheck
