#pragma once

#include "dep0/error.hpp"

#include <filesystem>
#include <type_traits>
#include <vector>

namespace dep0::linker::x86_64_linux {

expected<std::true_type> link(
    std::vector<std::filesystem::path> const& object_files,
    std::filesystem::path output_file_name
) noexcept;

} // namespace dep0::linker::x86_64_linux
