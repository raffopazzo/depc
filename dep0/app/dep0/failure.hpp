#pragma once

#include "dep0/error.hpp"

#include <filesystem>
#include <string_view>

/**
 * Helper functions that print an error message to stderr and return 1.
 * The error message has the general form `{filename}: error: {msg}: {error}`.
 * One or more fields can be missing depending on which overload is invoked.
 * @{
 */
int failure(std::string_view const msg);
int failure(std::filesystem::path const&, std::string_view const msg);
int failure(std::filesystem::path const&, std::string_view const msg, dep0::error_t const&);
int failure(std::filesystem::path const&, dep0::error_t const&);
/** @} */
