/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Set of functions to generate LLVM types.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include "private/context.hpp"
#include "private/proto.hpp"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>

namespace dep0::llvmgen {

/** @brief Generate an LLVM function type for the given prototype. */
llvm::FunctionType* gen_func_type(global_ctx_t&, llvm_func_proto_t const&);

/** @brief Return the LLVM integer type of the given bit width. */
llvm::IntegerType* gen_type(global_ctx_t&, ast::width_t);

/**
 * @brief Generate the LLVM type for the given type expression, which must be a 1st order type.
 * @param type Must be an expression of sort `typename_t` and it must represent a 1st order type.
 * @see
 *   - @ref first_order_types
 *   - @ref first_order_types.hpp
 */
llvm::Type* gen_type(global_ctx_t&, typecheck::expr_t const& type);

} // namespace dep0::llvmgen
