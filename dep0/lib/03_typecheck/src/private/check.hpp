#pragma once

#include "proof_state.hpp" // also in `private/`

#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/error.hpp"

#include "dep0/parser/ast.hpp"

#include <optional>
#include <vector>

namespace dep0::typecheck {

expected<type_def_t> check_type_def(context_t&, parser::type_def_t const&);
expected<func_def_t> check_func_def(context_t&, parser::func_def_t const&);
expected<expr_t> check_type(context_t const&, parser::expr_t const&);
expected<body_t> check_body(proof_state_t&, parser::body_t const&);
expected<stmt_t> check_stmt(proof_state_t&, parser::stmt_t const&);
expected<expr_t> check_expr(context_t const&, parser::expr_t const&, sort_t const& expected_type);
expected<expr_t> check_abs(
    context_t const&,
    parser::expr_t::abs_t const&,
    source_loc_t const&,
    std::optional<source_text> const&);
expected<expr_t> check_numeric_expr(
    context_t const&,
    parser::expr_t::numeric_constant_t const&,
    source_loc_t const&,
    expr_t const& expected_type);
expected<expr_t> check_pi_type(context_t&, std::vector<parser::func_arg_t> const&, parser::expr_t const& ret_ty);

} // namespace dep0::typecheck
