#pragma once

#include "dep0/transform/transform.hpp"

#include "dep0/scope_map.hpp"

namespace dep0::transform {

namespace delta_reduction {
    using context_t = scope_map<source_text, std::variant<typecheck::expr_t::abs_t, struct something_else_t>>;
    struct something_else_t{};
}

bool delta_reduce(delta_reduction::context_t const&, typecheck::func_def_t&);
bool delta_reduce(delta_reduction::context_t const&, typecheck::body_t&);
bool delta_reduce(delta_reduction::context_t const&, typecheck::stmt_t&);
bool delta_reduce(delta_reduction::context_t const&, typecheck::expr_t&);
bool delta_reduce(delta_reduction::context_t const&, typecheck::type_t&);

// TODO could also define delta-normalization but currently it's not needed because we used beta-delta normalization

} // namespace dep0::transform
