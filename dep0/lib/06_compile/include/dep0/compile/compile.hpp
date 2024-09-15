#pragma once

#include "dep0/error.hpp"
#include "dep0/temp_file.hpp"

#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>

namespace dep0::compile {

/**
 * Compile the given llvm module into an assembly file for the given target machine and
 * return a temporary file object associated with it.
 *
 * @remarks It is the caller responsibility to keep the temporary file if required.
 */
expected<temp_file_t> compile_only(llvm::Module&, llvm::TargetMachine&) noexcept;

/**
 * Compile and assemble the given llvm module into an object file for the given target machine and
 * return a temporary file object associated with it.
 *
 * @remarks It is the caller responsibility to keep the temporary file if required.
 */
expected<temp_file_t> compile_and_assemble(llvm::Module&, llvm::TargetMachine&) noexcept;

} // namespace dep0::compile
