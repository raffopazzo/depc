#pragma once

#include "dep0/transform/transform.hpp"

#include "dep0/typecheck/ast.hpp"

#include "dep0/ast/delta_reduction.hpp"

namespace dep0::transform {

struct beta_delta_normalization_t
{
    ast::delta_reduction::context_t<typecheck::properties_t> ctx;
    expected<std::true_type> operator()(typecheck::module_t&) const;
};
static_assert(Transform<beta_delta_normalization_t>);

} // namespace dep0::transform
