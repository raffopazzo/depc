#pragma once

#include "dep0/transform/delta_reduction.hpp"
#include "dep0/transform/transform.hpp"

#include "dep0/typecheck/ast.hpp"

namespace dep0::transform {

bool beta_delta_normalize(delta_reduction::context_t const&, typecheck::module_t&);
bool beta_delta_normalize(delta_reduction::context_t const&, typecheck::func_def_t&);
bool beta_delta_normalize(delta_reduction::context_t const&, typecheck::body_t&);
bool beta_delta_normalize(delta_reduction::context_t const&, typecheck::stmt_t&);
bool beta_delta_normalize(delta_reduction::context_t const&, typecheck::expr_t&);
bool beta_delta_normalize(delta_reduction::context_t const&, typecheck::type_t&);

struct beta_delta_normalization_t
{
    delta_reduction::context_t ctx;
    expected<std::true_type> operator()(typecheck::module_t&) const;
};
static_assert(Transform<beta_delta_normalization_t>);

} // namespace dep0::transform
