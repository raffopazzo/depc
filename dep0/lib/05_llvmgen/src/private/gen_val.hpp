#pragma once

#include "private/context.hpp"

#include "dep0/typecheck/ast.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

namespace dep0::llvmgen {

/**
 * Generates an LLVM value for the given type and number.
 */
llvm::Value* gen_val(llvm::IntegerType*, boost::multiprecision::cpp_int const&);

/**
 * Generates an LLVM value for the given expression.
 *
 * @param dest
 *      If this parameter is not nullptr, this function will emit appropriate IR instructions
 *      to store/memcpy/memset the generated value at the runtime location pointed by this LLVM value,
 *      which must therefore be of an appropriate pointer type.
 *
 * @return The value generated or `dest` if it was not nullptr.
 */
llvm::Value* gen_val(
    global_ctx_t&,
    local_ctx_t const&,
    llvm::IRBuilder<>&,
    typecheck::expr_t const&,
    llvm::Value* dest);

/**
 * Generates an LLVM value from a runtime function call corresponding to the given application expression.
 *
 * @param dest
 *      If this parameter is not nullptr, this function will emit appropriate IR instructions
 *      to store/memcpy/memset the result of the function call at the runtime location pointed by
 *      this LLVM value, which must therefore be of an appropriate pointer type.
 *
 * @return The generated LLVM value or `dest` if it was not nullptr.
 */
llvm::Value* gen_func_call(
    global_ctx_t&,
    local_ctx_t const&,
    llvm::IRBuilder<>&,
    typecheck::expr_t::app_t const&,
    llvm::Value* dest);

} // namespace dep0::llvmgen
