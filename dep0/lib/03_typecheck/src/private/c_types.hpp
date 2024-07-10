#pragma once

#include "dep0/parser/ast.hpp"

#include "dep0/error.hpp"

#include <type_traits>

namespace dep0::typecheck {

/**
 * Checks whether the given type is representable by a type in C.
 * @note This is required for extern function declarations.
 */
dep0::expected<std::true_type> is_c_type(parser::expr_t const& type);

/**
 * Checks whether the given function type is representable by a type in C.
 *
 * @param origin
 *      The location in the source file where the given pi-type was found.
 *      If it is not a valid C-function type, it will be copied in the error message.
 */
dep0::expected<std::true_type> is_c_func_type(parser::expr_t::pi_t const&, source_loc_t origin);

} // namespace dep0::typecheck
