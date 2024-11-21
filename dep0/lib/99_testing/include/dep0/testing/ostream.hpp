/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/ast/ast.hpp"

#include <iostream>

namespace dep0::ast {

std::ostream& operator<<(std::ostream&, is_mutable_t);

} // namespace dep0::ast
