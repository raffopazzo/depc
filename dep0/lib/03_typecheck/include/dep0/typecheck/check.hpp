#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/error.hpp"

#include "dep0/parser/ast.hpp"

namespace dep0::typecheck {

expected<module_t> check(parser::module_t const&) noexcept;

} // namespace dep0::typecheck
