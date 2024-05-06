#pragma once

#include "private/context.hpp"
#include "private/proto.hpp"

#include "dep0/typecheck/ast.hpp"
#include "dep0/source.hpp"

#include <llvm/IR/Value.h>

namespace dep0::llvmgen {

/**
 * Generate an extern function declaration of the given prototype.
 *
 * @param name
 *      The name to assign to the extern function declaration.
 */
void gen_extern_decl(
    global_ctx_t&,
    typecheck::expr_t::global_t const& name,
    llvm_func_proto_t const&);

/**
 * Generate an LLVM function declaration of the given prototype.
 *
 * @param name
 *      The name to assign to the LLVM function that will be generated.
 */
void gen_func_decl(
    global_ctx_t&,
    typecheck::expr_t::global_t const& name,
    llvm_func_proto_t const&);

/**
 * Generate an LLVM function from an anonymous lambda abstraction of the given prototype.
 *
 * @return An LLVM value which is the pointer to the generated LLVM function.
 */
llvm::Value* gen_func(
    global_ctx_t&,
    llvm_func_proto_t const&,
    typecheck::expr_t::abs_t const&);

/**
 * Generate an LLVM function for a named abstraction of the given prototype.
 *
 * @param name
 *      The name to assign to the LLVM function that will be generated.
 */
void gen_func(
    global_ctx_t&,
    typecheck::expr_t::global_t const& name,
    llvm_func_proto_t const&,
    typecheck::expr_t::abs_t const&);

} // namespace dep0::llvmgen
