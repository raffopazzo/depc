/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Helper functions that print an error message to stderr and return 1.
 * 
 * The error message has the general form `{filename}: error: {msg}: {error}`.
 * One or more fields can be missing depending on which overload is invoked.
 */
#pragma once

#include "dep0/error.hpp"

#include <filesystem>
#include <string_view>

int failure(std::string_view const msg);
int failure(std::filesystem::path const&, std::string_view const msg);
int failure(std::filesystem::path const&, std::string_view const msg, dep0::error_t const&);
int failure(std::filesystem::path const&, dep0::error_t const&);
