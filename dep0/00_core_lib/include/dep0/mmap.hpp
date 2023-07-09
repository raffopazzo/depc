#pragma once

#include "dep0/error.hpp"
#include "dep0/source.hpp"

#include <filesystem>

namespace dep0 {

expected<source_text> mmap(std::filesystem::path);

} // namespace dep0
