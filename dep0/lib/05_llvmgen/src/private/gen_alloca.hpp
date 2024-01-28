#pragma once

#include "private/context.hpp"

#include "dep0/typecheck/ast.hpp"

#include <llvm/IR/Instruction.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include <variant>

namespace dep0::llvmgen {

namespace needs_alloca_result
{
    /**
     * The type does not require an allocation.
     */
    struct no_t{};

    /**
     * The type requires an allocation because it is an array.
     */
    struct array_t
    {
        typecheck::expr_t const& type;
        typecheck::expr_t const& size;
    };
}
using needs_alloca_result_t =
    std::variant<
        needs_alloca_result::no_t,
        needs_alloca_result::array_t>;

/**
 * Decides whether the given type expression requires an allocation or not.
 *
 * @param type  The type expression to check, which must have sort `typename_t`.
 *
 * @return The result of the decision.
 */
needs_alloca_result_t needs_alloca(typecheck::expr_t const& type);

/**
 * Checks whether the given type expression requires an allocation or not.
 *
 * @param type  The type expression to check, which must have sort `typename_t`.
 *
 * @return True if allocation is required for the given type, false otherwise.
 */
bool is_alloca_needed(typecheck::expr_t const& type);

/**
 * Generate an IR alloca instruction.
 *
 * @param builder      The builder that will be used to generate all the required LLVM instructions and values.
 * @param element_type The type expression representing the type of the element that needs to be allocated.
 * @param size         An LLVM value holding the number of elements to allocate.
 *
 * @return The generated alloca instruction.
 */
llvm::Instruction* gen_alloca(
    global_context_t&,
    local_context_t const&,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t const& element_type,
    llvm::Value* const size);

/**
 * Determine if the given type expression requires an allocation,
 * and if it does then emit an alloca instruction along with all instructions needed to compute sizes etc.
 *
 * @param builder   The builder that will be used to generate all the LLVM instructions and values.
 * @param type      The type expression to check; for example an `array_t(i32_t, n)`.
 *
 * @return The generated alloca instruction or nullptr if allocation is not required for the given type.
 */
llvm::Instruction* gen_alloca_if_needed(
    global_context_t&,
    local_context_t const&,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t const& type);

} // namespace dep0::llvmgen
