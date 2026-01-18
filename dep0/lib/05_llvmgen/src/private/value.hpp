/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Contains the definition of `dep0::llvmgen::value_t`.
 */
#pragma once

#include <llvm/IR/Value.h>

#include "dep0/ast/mutable.hpp"

namespace dep0::llvmgen {

/** TODO */
class value_t
{
    llvm::Value* m_value;
    bool m_is_mutable;
    bool m_is_pass_by_ptr;

public:
    value_t(llvm::Value*, ast::is_mutable_t, bool is_pass_by_ptr);

    value_t(value_t const&) = default;
    value_t(value_t&&) = default;

    operator llvm::Value*() const { return m_value; }

    bool is_mutable() const { return m_is_mutable; }
    bool is_pass_by_ptr() const { return m_is_pass_by_ptr; }
    bool is_pass_by_val() const { return not m_is_pass_by_ptr; }
};

} // namespace dep0::llvmgen
