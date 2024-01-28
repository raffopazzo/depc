#pragma once

#include "dep0/typecheck/ast.hpp"

#include <optional>

namespace dep0::typecheck {

std::optional<sort_t> rewrite(expr_t const& from, expr_t const& to, sort_t const&);

} // namespace dep0::typecheck
