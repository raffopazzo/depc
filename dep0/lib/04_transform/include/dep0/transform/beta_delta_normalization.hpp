#pragma once

#include "dep0/transform/transform.hpp"

#include "dep0/typecheck/ast.hpp"

namespace dep0::transform {

struct beta_delta_normalization_t
{
    expected<std::true_type> operator()(typecheck::module_t&) const;
};
static_assert(Transform<beta_delta_normalization_t>);

} // namespace dep0::transform
