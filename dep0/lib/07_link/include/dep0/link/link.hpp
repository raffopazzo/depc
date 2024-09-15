#pragma once

#include "dep0/error.hpp"
#include "dep0/temp_file.hpp"

#include <llvm/ADT/Triple.h>

#include <filesystem>
#include <vector>

namespace dep0::link {

/**
 * Link all given object files into the final executable file and
 * return a temporary file object associated with it.
 *
 * @param object_files
 *      The names of all object files to link together.
 *
 * @param target
 *      The cpu-vendor-platform triple of the machine on which the executable will be run.
 *
 * @param host
 *      The cpu-vendor-platform triple of the machine on which compilation is happening.
 *
 * @remarks It is the caller responsibility to keep the temporary file if required.
 */
expected<temp_file_t> link(
    std::vector<std::filesystem::path> const& object_files,
    llvm::Triple target,
    llvm::Triple host
) noexcept;

} // namespace dep0::link
