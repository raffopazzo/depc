/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/root_var.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

namespace {

expr_t::var_t const* root_var(expr_t const& x)
{
    return match(
        x.value,
        [] (expr_t::typename_t) { return nullptr; },
        [] (expr_t::true_t) { return nullptr; },
        [] (expr_t::auto_t) { return nullptr; },
        [] (expr_t::bool_t) { return nullptr; },
        [] (expr_t::cstr_t) { return nullptr; },
        [] (expr_t::unit_t) { return nullptr; },
        [] (expr_t::i8_t) { return nullptr; },
        [] (expr_t::i16_t) { return nullptr; },
        [] (expr_t::i32_t) { return nullptr; },
        [] (expr_t::i64_t) { return nullptr; },
        [] (expr_t::u8_t) { return nullptr; },
        [] (expr_t::u16_t) { return nullptr; },
        [] (expr_t::u32_t) { return nullptr; },
        [] (expr_t::u64_t) { return nullptr; },
        [] (expr_t::boolean_constant_t const&) { return nullptr; },
        [] (expr_t::numeric_constant_t const&) { return nullptr; },
        [] (expr_t::string_literal_t const&) { return nullptr; },
        [] (expr_t::boolean_expr_t const&) { return nullptr; },
        [] (expr_t::relation_expr_t const&) { return nullptr; },
        [] (expr_t::arith_expr_t const&) { return nullptr; },
        [] (expr_t::var_t const& x) { return &x; },
        [] (expr_t::global_t const&) { return nullptr; },
        [] (expr_t::app_t const&) { return nullptr; },
        [] (expr_t::abs_t const&) { return nullptr; },
        [] (expr_t::pi_t const&) { return nullptr; },
        [] (expr_t::sigma_t const&) { return nullptr; },
        [] (expr_t::ref_t) { return nullptr; },
        [] (expr_t::scope_t) { return nullptr; },
        [] (expr_t::addressof_t const&) { return nullptr; },
        [] (expr_t::deref_t const&) { return nullptr; },
        [] (expr_t::scopeof_t const&) { return nullptr; },
        [] (expr_t::array_t const&) { return nullptr; },
        [] (expr_t::init_list_t const&) { return nullptr; },
        [] (expr_t::member_t const& x) { return root_var(x.object.get()); },
        [] (expr_t::subscript_t const& x) { return root_var(x.object.get()); },
        [] (expr_t::because_t const& x) { return root_var(x.value.get()); });
}

} // namespace

expr_t::var_t const* root_var(expr_t::member_t const& x)
{
    return root_var(x.object.get());
}

expr_t::var_t const* root_var(expr_t::subscript_t const& x)
{
    return root_var(x.object.get());
}

} // namespace dep0::typecheck
