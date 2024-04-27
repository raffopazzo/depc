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
 * This enum is passed to @see gen to control whether verification of the LLVM module should be performed or skipped.
 * Skipping verification is useful when debugging the codegen logic,
 * allowing the broken module to be saved to a file for manual inspection.
 */
enum class verify_t { yes, no };

/**
 * Generate an LLVM module from a legal module; optionally skip verification of the LLVM module.
 *
 * @param ctx
 *      The LLVM context used during codegen; it holds LLVM types, the target machine, etc.
 *
 * @param module_name
 *      The name to assign to the generated LLVM module.
 *
 * @param module
 *      The legal DepC module for which to emit an LLVM module.
 *
 * @param verify
 *      Controls whether verification of the LLVM module should be performed (default) or skipped;
 *      skipping verification is useful when debugging the codegen logic,
 *      allowing the broken module to be saved to a file for manual inspection.
 *
 * @return The generated LLVM module or an error; if verification was performed this will be a valid LLVM module.
 */
expected<unique_ref<llvm::Module>> gen(
    llvm::LLVMContext& ctx,
    std::string_view module_name,
    typecheck::module_t const& module,
    verify_t verify = verify_t::yes
) noexcept;

}
