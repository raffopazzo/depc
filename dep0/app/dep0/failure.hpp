#pragma once

#include "dep0/error.hpp"
#include "dep0/typecheck/error.hpp"

#include <filesystem>
#include <string_view>

int failure(std::string_view const msg);
int failure(std::filesystem::path const&, std::string_view const msg);
int failure(std::filesystem::path const&, std::string_view const msg, dep0::error_t const&);
int failure(std::filesystem::path const&, std::string_view const msg, dep0::typecheck::error_t const&);
