#pragma once

#include "dep0/error.hpp"

#include <llvm/ADT/Triple.h>

#include <filesystem>
#include <type_traits>
#include <vector>

namespace dep0::linker {

/**
 * Link all given object files into the final executable file.
 *
 * @param object_files
 *      The names of all object files to link together.
 *
 * @param target_triple
 *      The cpu-vendor-platform triple of the machine on which the executable will be run.
 *
 * @param host_triple
 *      The cpu-vendor-platform triple of the machine on which compilation is happening.
 *
 * @param output_file_name
 *      The preferred file name for the output executable.
 */
expected<std::true_type> link(
    std::vector<std::filesystem::path> const& object_files,
    llvm::Triple target_triple,
    llvm::Triple host_triple,
    std::filesystem::path output_file_name
) noexcept;

} // namespace dep0::linker
