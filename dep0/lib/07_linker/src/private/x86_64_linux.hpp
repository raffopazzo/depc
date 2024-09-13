#pragma once

#include "dep0/error.hpp"
#include "dep0/temp_file.hpp"

#include <filesystem>
#include <type_traits>
#include <vector>

namespace dep0::linker::x86_64_linux {

expected<temp_file_t> link(std::vector<std::filesystem::path> const& object_files) noexcept;

} // namespace dep0::linker::x86_64_linux
