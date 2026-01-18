/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/value.hpp"

namespace dep0::llvmgen {

value_t::value_t(llvm::Value* const value, ast::is_mutable_t const is_mutable, bool const is_pass_by_ptr) :
    m_value(value),
    m_is_mutable(is_mutable == ast::is_mutable_t::yes),
    m_is_pass_by_ptr(is_pass_by_ptr)
{
}

} // namespace dep0::llvmgen
