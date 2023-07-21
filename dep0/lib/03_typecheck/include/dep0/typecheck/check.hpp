#pragma once

#include "dep0/typecheck/tt/context.hpp"
#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/error.hpp"

#include "dep0/parser/ast.hpp"

#include "dep0/error.hpp"

#include <optional>

namespace dep0::typecheck {

expected<module_t> check(parser::module_t const&);

} // namespace dep0::typecheck
