/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `dep0::mmap()`.
 */
#pragma once

#include "dep0/error.hpp"
#include "dep0/source.hpp"

#include <filesystem>

namespace dep0 {

/**
 * @brief Map a file in the process' memory and returns a `dep0::source_text` object over its content or an error.
 * 
 * The file will be automatically unmapped when the `dep0::source_text` object is destroyed.
 */
expected<source_text> mmap(std::filesystem::path);

} // namespace dep0
