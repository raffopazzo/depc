/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include <optional>

namespace dep0::typecheck {

std::optional<sort_t> rewrite(expr_t const& from, expr_t const& to, sort_t const&);

} // namespace dep0::typecheck
