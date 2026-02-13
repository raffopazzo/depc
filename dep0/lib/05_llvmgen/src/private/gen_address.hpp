/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Contains the declaration of `gen_address()`.
 */
#pragma once

#include "private/context.hpp"

#include "dep0/typecheck/ast.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

namespace dep0::llvmgen {

/** @brief Genreates the runtime memory address of the given subscript exprresion. */
llvm::Value*
gen_address(
    global_ctx_t&,
    local_ctx_t&,
    llvm::IRBuilder<>&,
    typecheck::expr_t::subscript_t const&);

std::pair<llvm::Value*, llvm::Type*>
gen_field_address(
    global_ctx_t&,
    local_ctx_t&,
    llvm::IRBuilder<>&,
    typecheck::expr_t::member_t const&);

} // namespace dep0::llvmgen
