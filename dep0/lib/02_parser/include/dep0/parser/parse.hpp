#pragma once

#include "dep0/parser/ast.hpp"

#include "dep0/error.hpp"

#include <filesystem>

namespace dep0::parser {

expected<module_t> parse(std::filesystem::path const&);

}
