/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/typecheck/is_mutable.hpp"

#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::typecheck {

namespace impl {

static bool is_mutable(expr_t::typename_t const&) { return false; }
static bool is_mutable(expr_t::true_t const&) { return false; }
static bool is_mutable(expr_t::auto_t const&) { return false; }
static bool is_mutable(expr_t::bool_t const&) { return false; }
static bool is_mutable(expr_t::cstr_t const&) { return false; }
static bool is_mutable(expr_t::unit_t const&) { return false; }
static bool is_mutable(expr_t::i8_t const&) { return false; }
static bool is_mutable(expr_t::i16_t const&) { return false; }
static bool is_mutable(expr_t::i32_t const&) { return false; }
static bool is_mutable(expr_t::i64_t const&) { return false; }
static bool is_mutable(expr_t::u8_t const&) { return false; }
static bool is_mutable(expr_t::u16_t const&) { return false; }
static bool is_mutable(expr_t::u32_t const&) { return false; }
static bool is_mutable(expr_t::u64_t const&) { return false; }
static bool is_mutable(expr_t::boolean_constant_t const&) { return false; }
static bool is_mutable(expr_t::numeric_constant_t const&) { return false; }
static bool is_mutable(expr_t::string_literal_t const&) { return false; }
static bool is_mutable(expr_t::boolean_expr_t const&);
static bool is_mutable(expr_t::relation_expr_t const&);
static bool is_mutable(expr_t::arith_expr_t const&);
static bool is_mutable(expr_t::var_t const&) { return false; }
static bool is_mutable(expr_t::global_t const&) { return false; }
static bool is_mutable(expr_t::abs_t const&);
static bool is_mutable(expr_t::pi_t const&);
static bool is_mutable(expr_t::sigma_t const&) { return false; }
static bool is_mutable(expr_t::array_t const&) { return false; }
static bool is_mutable(expr_t::init_list_t const&);
static bool is_mutable(expr_t::member_t const&);
static bool is_mutable(expr_t::subscript_t const&);
static bool is_mutable(expr_t::because_t const&);

static bool is_mutable(expr_t::boolean_expr_t const& x)
{
    return match(
        x.value,
        [] (expr_t::boolean_expr_t::not_t const& x) { return is_mutable(x.expr.get()); },
        [] (auto const& x) { return is_mutable(x.lhs.get()) or is_mutable(x.rhs.get()); });
}

static bool is_mutable(expr_t::relation_expr_t const& x)
{
    return match(x.value, [] (auto const& x) { return is_mutable(x.lhs.get()) or is_mutable(x.rhs.get()); });
}

static bool is_mutable(expr_t::arith_expr_t const& x)
{
    return match(x.value, [] (auto const& x) { return is_mutable(x.lhs.get()) or is_mutable(x.rhs.get()); });
}

static bool is_mutable(expr_t::abs_t const&)
{
    // consider something like `[] () mutable -> i32_t { return rand(); }`, this expression in itself is not mutable;
    // invoking this lambda is mutable but the expression generating the lambda is not;
    // though in future it might become, for example `[x=rand()] () -> i32_t { return x; }` is mutable;
    // each time this expression is evaluated a different lambda might be generated
    return false;
}

static bool is_mutable(expr_t::pi_t const&)
{
    // consider something like `() mutable -> i32_t`, this expression in itself is not mutable
    return false;
}

static bool is_mutable(expr_t::init_list_t const& x)
{
    return std::ranges::any_of(x.values, [] (expr_t const& v) { return is_mutable(v); });
}

static bool is_mutable(expr_t::member_t const& x)
{
    return is_mutable(x.object.get());
}

static bool is_mutable(expr_t::subscript_t const& x)
{
    return is_mutable(x.object.get()) or is_mutable(x.index.get());
}

static bool is_mutable(expr_t::because_t const& x)
{
    // reason can contain a mutable expression but it is erased at runtime,
    // so what matters is whether the value itself is mutable
    return is_mutable(x.value.get());
}

} // namespace impl

bool is_mutable(expr_t const& x)
{
    return match(
        x.value,
        [] (expr_t::app_t const& app) { return is_mutable(app); },
        [] (auto const& x) { return impl::is_mutable(x); });
}

bool is_mutable(expr_t::app_t const& x)
{
    auto const& func_type = std::get<expr_t>(x.func.get().properties.sort.get());
    return std::get<expr_t::pi_t>(func_type.value).is_mutable == ast::is_mutable_t::yes or
        std::ranges::any_of(x.args, [] (expr_t const& arg) { return is_mutable(arg); });
}

} // namespace dep0::typecheck
