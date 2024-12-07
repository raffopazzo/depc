/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `get_sign_ext_attribute()`.
 */
#pragma once

#include "private/context.hpp"

#include "dep0/typecheck/ast.hpp"

#include <llvm/IR/Attributes.h>

namespace dep0::llvmgen {

/** @brief Return the sign extension attribute for the given type expression, or `None` if not an integral type. */
llvm::Attribute::AttrKind get_sign_ext_attribute(global_ctx_t const&, typecheck::expr_t const& type);

} // namespace dep0::llvmgen

