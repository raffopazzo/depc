#pragma once

#include "dep0/error.hpp"

#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace dep0::linker::x86_64_linux {

expected<std::true_type> link(
    std::vector<std::string> const& object_files,
    std::string_view output_file_name
) noexcept;

} // namespace dep0::linker::x86_64_linux
