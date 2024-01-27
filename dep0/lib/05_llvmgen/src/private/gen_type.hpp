#pragma once

#include "dep0/typecheck/ast.hpp"

#include "private/context.hpp"
#include "private/proto.hpp"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>

namespace dep0::llvmgen {

/**
 * Generate an LLVM function type for the given prototype.
 */
llvm::FunctionType* gen_func_type(global_context_t&, local_context_t const&, llvm_func_proto_t const&);

/**
 * Generate the LLVM type for the given type expression, which must be a 1st order type.
 */
llvm::Type* gen_type(global_context_t&, local_context_t const&, typecheck::expr_t const&);

} // namespace dep0::llvmgen
