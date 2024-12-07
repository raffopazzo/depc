/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Entry-point of the linking stage, run after the compile stage.
 */
#pragma once

#include "dep0/error.hpp"
#include "dep0/temp_file.hpp"

#include <llvm/ADT/Triple.h>

#include <filesystem>
#include <vector>

namespace dep0::link {

/**
 * @brief Link all given object files into the final executable file.
 * @param object_files The paths to all object files to link together.
 * @param target The cpu-vendor-platform triple of the machine on which the executable will be run.
 * @param host The cpu-vendor-platform triple of the machine on which compilation is happening.
 * @return A temporary file containing the final executable code.
 * @remarks It is the caller responsibility to keep the temporary file if required.
 */
expected<temp_file_t> link(
    std::vector<std::filesystem::path> const& object_files,
    llvm::Triple target,
    llvm::Triple host
) noexcept;

} // namespace dep0::link
