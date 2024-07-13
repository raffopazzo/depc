#pragma once

#include "dep0/parser/ast.hpp"

#include "dep0/error.hpp"
#include "dep0/source.hpp"

#include <filesystem>

namespace dep0::parser {

/** Open the given file and parse its content. */
expected<module_t> parse(std::filesystem::path const&) noexcept;

/** Parse the given source text. */
expected<module_t> parse(source_text) noexcept;

}
