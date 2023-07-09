#pragma once

#include "dep0/parser/parse_tree.hpp"

#include "dep0/error.hpp"

#include <filesystem>

namespace dep0::parser {

expected<parse_tree> parse(std::filesystem::path const&);

}
