/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Specialisation of the linking stage for `x86_64-linux` targets.
 */
#pragma once

#include "dep0/error.hpp"
#include "dep0/temp_file.hpp"

#include <filesystem>
#include <vector>

namespace dep0::link::x86_64_linux {

/** @brief Link all given object files into a final executable for `x86_64-linux`. */
expected<temp_file_t> link(std::vector<std::filesystem::path> const& object_files) noexcept;

} // namespace dep0::link::x86_64_linux
