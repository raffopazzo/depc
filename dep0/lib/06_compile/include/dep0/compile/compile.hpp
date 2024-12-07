/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Functions used at the compile stage, after an LLVM module has been generated.
 */
#pragma once

#include "dep0/error.hpp"
#include "dep0/temp_file.hpp"

#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>

namespace dep0::compile {

/**
 * @brief Compile the given llvm module into an assembly file for the given target machine.
 * @return A temporary file object containing the generated assembly code.
 * @remarks It is the caller responsibility to keep the temporary file if required.
 */
expected<temp_file_t> compile_only(llvm::Module&, llvm::TargetMachine&) noexcept;

/**
 * @brief Compile and assemble the given llvm module into an object file for the given target machine.
 * @return A temporary file containing the generated machine code.
 * @remarks It is the caller responsibility to keep the temporary file if required.
 */
expected<temp_file_t> compile_and_assemble(llvm::Module&, llvm::TargetMachine&) noexcept;

} // namespace dep0::compile
