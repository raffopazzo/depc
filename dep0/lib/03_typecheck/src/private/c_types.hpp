/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Functions that check whether a type in DepC is representable by a type in C.
 */
#pragma once

#include "dep0/parser/ast.hpp"

#include "dep0/error.hpp"

#include <type_traits>

namespace dep0::typecheck {

/**
 * @brief Checks whether the given type is representable by a type in C.
 * @remarks This is required for extern function declarations.
 */
dep0::expected<std::true_type> is_c_type(parser::expr_t const& type);

/**
 * @brief Checks whether the given function type is representable by a type in C.
 *
 * @param origin
 *      The location in the source file where the given pi-type was found.
 *      If it is not a valid C-function type, it will be copied in the error message.
 */
dep0::expected<std::true_type> is_c_func_type(parser::expr_t::pi_t const&, source_loc_t origin);

} // namespace dep0::typecheck
