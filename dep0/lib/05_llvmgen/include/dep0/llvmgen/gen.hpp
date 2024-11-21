/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"
#include "dep0/unique_ref.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <memory>
#include <string_view>

namespace dep0::llvmgen {

/**
 * Generate an LLVM module from a legal DepC module.
 *
 * @param ctx           The LLVM context used during codegen; it holds LLVM types, the target machine, etc.
 * @param module_name   The name to assign to the generated LLVM module.
 *
 * @remarks
 *      This function cannot simply return an `expected<llvm::Module>` because
 *      the llvm objects created during codegen will store pointers/references to the llvm module which
 *      can be invalidated if the llvm module gets moved around.
 */
expected<unique_ref<llvm::Module>>
gen(llvm::LLVMContext& ctx, std::string_view module_name, typecheck::module_t const&) noexcept;

/**
 * Like `gen()` but the generated LLVM module is unverified so it may be invalid.
 * This helps debugging `gen()` by allowing the broken module to be saved to a file for manual inspection.
 */
expected<unique_ref<llvm::Module>>
gen_unverified(llvm::LLVMContext&, std::string_view, typecheck::module_t const&) noexcept;

} // namespace dep0::llvmgen
