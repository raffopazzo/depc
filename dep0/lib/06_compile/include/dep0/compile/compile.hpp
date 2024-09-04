#pragma once

#include "dep0/error.hpp"

#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>

#include <filesystem>

namespace dep0::compile {

// TODO add variants that create temporary files instead

/**
 * Compile the given llvm module into an assembly file for the given target machine.
 *
 * @param output_file_name
 *      The desired name of the output assembly file.
 *
 * @return The actual name of the output file, which is equal to the desired name on success.
 */
expected<std::filesystem::path>
compile_only(llvm::Module&, llvm::TargetMachine&, std::filesystem::path const& output_file_name);

/**
 * Compile and assemble the given llvm module into an object file for the given target machine
 *
 * @param output_file_name
 *      The desired name of the output object file.
 *
 * @return The actual name of the output file, which is equal to the desired name on success.
 */
expected<std::filesystem::path>
compile_and_assemble(llvm::Module&, llvm::TargetMachine&, std::filesystem::path const& output_file_name);

} // namespace dep0::compile
