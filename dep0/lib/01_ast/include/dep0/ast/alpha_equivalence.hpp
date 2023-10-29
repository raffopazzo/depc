#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/error.hpp"

namespace dep0::ast {

template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent(type_t<P> const&, type_t<P> const&);

} // namespace dep0::ast

#include "dep0/ast/alpha_equivalence_impl.hpp"
