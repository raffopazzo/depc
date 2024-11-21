/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/error.hpp"
#include "dep0/source.hpp"

#include <filesystem>

namespace dep0 {

expected<source_text> mmap(std::filesystem::path);

} // namespace dep0
