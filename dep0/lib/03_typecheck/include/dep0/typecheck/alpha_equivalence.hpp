#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/error.hpp"

namespace dep0::typecheck {

dep0::expected<std::true_type> is_alpha_equivalent(type_t const&, type_t const&);

} // namespace dep0::typecheck
