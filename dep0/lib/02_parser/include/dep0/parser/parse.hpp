/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Entry point of the parsing stage: the functions `dep0::parser::parse()`.
 */
#pragma once

#include "dep0/parser/ast.hpp"

#include "dep0/error.hpp"
#include "dep0/source.hpp"

#include <filesystem>

namespace dep0::parser {

/** @brief Open the given file and parse its content. */
expected<module_t> parse(std::filesystem::path const&) noexcept;

/** @brief Parse the given source text. */
expected<module_t> parse(source_text) noexcept;

}
