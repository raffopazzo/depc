#pragma once

#include "dep0/ast/ast.hpp"

#include "dep0/scope_map.hpp"

namespace dep0::ast {

namespace delta_reduction {
    template <Properties P>
    using definition_t = std::variant<
        typename expr_t<P>::abs_t,
        struct something_else_t>;

    template <Properties P>
    using context_t = scope_map<indexed_var_t, definition_t<P>>;

    struct something_else_t{};
}

// This family of functions only perform one-step delta-reduction, not a full delta-normalization.
template <Properties P> bool delta_reduce(delta_reduction::context_t<P> const&, func_def_t<P>&);
template <Properties P> bool delta_reduce(delta_reduction::context_t<P> const&, body_t<P>&);
template <Properties P> bool delta_reduce(delta_reduction::context_t<P> const&, stmt_t<P>&);
template <Properties P> bool delta_reduce(delta_reduction::context_t<P> const&, expr_t<P>&);
template <Properties P> bool delta_reduce(delta_reduction::context_t<P> const&, type_t<P>&);

} // namespace dep0::ast

#include "dep0/ast/delta_reduction_impl.hpp"
