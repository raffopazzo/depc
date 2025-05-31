/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/delta_unfold.hpp"

#include "private/cpp_int_add.hpp"
#include "private/cpp_int_div.hpp"
#include "private/cpp_int_mult.hpp"
#include "private/cpp_int_sub.hpp"
#include "private/derivation_rules.hpp"

#include "dep0/ast/find_member_field.hpp"
#include "dep0/typecheck/builtin_call.hpp"
#include "dep0/typecheck/is_mutable.hpp"

#include "dep0/destructive_self_assign.hpp"
#include "dep0/match.hpp"

#include <boost/core/ignore_unused.hpp>
#include <boost/hana.hpp>

#include <cassert>
#include <limits>
#include <optional>

namespace hana = boost::hana;

using cpp_int = boost::multiprecision::cpp_int;

namespace dep0::typecheck {

namespace impl {

static bool delta_unfold(env_t const&, ctx_t const&, stmt_t&);
static bool delta_unfold(env_t const&, ctx_t const&, stmt_t::if_else_t&);
static bool delta_unfold(env_t const&, ctx_t const&, stmt_t::return_t&);
static bool delta_unfold(env_t const&, ctx_t const&, stmt_t::impossible_t&);

static bool delta_unfold(env_t const&, ctx_t const&, expr_t::typename_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::true_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::auto_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::bool_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::cstr_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::unit_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::i8_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::i16_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::i32_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::i64_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::u8_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::u16_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::u32_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::u64_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::boolean_constant_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::numeric_constant_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::string_literal_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::boolean_expr_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::relation_expr_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::arith_expr_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::var_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::global_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::app_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::abs_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::pi_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::sigma_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::ref_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::scope_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::addressof_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::deref_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::scopeof_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::array_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::init_list_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::member_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::subscript_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::because_t&);

/**
 * Return the primitive delta-reduction of boolean expressions, boolean relations and numeric relations.
 * @{
 */
static bool reduce(hana::type<expr_t::boolean_expr_t::and_t>, bool const a, bool const b) { return a and b; }
static bool reduce(hana::type<expr_t::boolean_expr_t::or_t>, bool const a, bool const b) { return a or b; }

static bool reduce(hana::type<expr_t::relation_expr_t::eq_t>, bool const a, bool const b) { return a == b; }
static bool reduce(hana::type<expr_t::relation_expr_t::neq_t>, bool const a, bool const b) { return a != b; }
static bool reduce(hana::type<expr_t::relation_expr_t::gt_t>, bool const a, bool const b) { return a > b; }
static bool reduce(hana::type<expr_t::relation_expr_t::gte_t>, bool const a, bool const b) { return a >= b; }
static bool reduce(hana::type<expr_t::relation_expr_t::lt_t>, bool const a, bool const b) { return a < b; }
static bool reduce(hana::type<expr_t::relation_expr_t::lte_t>, bool const a, bool const b) { return a <= b; }

static bool reduce(hana::type<expr_t::relation_expr_t::eq_t>, cpp_int const& a, cpp_int const& b) { return a == b; }
static bool reduce(hana::type<expr_t::relation_expr_t::neq_t>, cpp_int const& a, cpp_int const& b) { return a != b; }
static bool reduce(hana::type<expr_t::relation_expr_t::gt_t>, cpp_int const& a, cpp_int const& b) { return a > b; }
static bool reduce(hana::type<expr_t::relation_expr_t::gte_t>, cpp_int const& a, cpp_int const& b) { return a >= b; }
static bool reduce(hana::type<expr_t::relation_expr_t::lt_t>, cpp_int const& a, cpp_int const& b) { return a < b; }
static bool reduce(hana::type<expr_t::relation_expr_t::lte_t>, cpp_int const& a, cpp_int const& b) { return a <= b; }
/** @} */

/**
 * Return the primitive delta-reduction of arithmetic expressions, like `a + b`, in the domain of the given type.
 * @{
 */
static cpp_int
reduce(env_t const&, hana::type<expr_t::arith_expr_t::plus_t>, cpp_int const& a, cpp_int const& b, expr_t const& ty);

static cpp_int
reduce(env_t const&, hana::type<expr_t::arith_expr_t::minus_t>, cpp_int const& a, cpp_int const& b, expr_t const& ty);

static cpp_int
reduce(env_t const&, hana::type<expr_t::arith_expr_t::mult_t>, cpp_int const& a, cpp_int const& b, expr_t const& ty);

static cpp_int
reduce(env_t const&, hana::type<expr_t::arith_expr_t::div_t>, cpp_int const& a, cpp_int const& b, expr_t const& ty);
/** @} */

bool delta_unfold(env_t const& env, ctx_t const& ctx, stmt_t& stmt)
{
    return match(stmt.value, [&] (auto& x) { return delta_unfold(env, ctx, x); });
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, stmt_t::if_else_t& if_)
{
    return delta_unfold(env, ctx, if_.cond)
        or delta_unfold(env, ctx, if_.true_branch)
        or if_.false_branch and delta_unfold(env, ctx, *if_.false_branch);
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, stmt_t::return_t& ret)
{
    return ret.expr and delta_unfold(env, ctx, *ret.expr);
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, stmt_t::impossible_t& x)
{
    return x.reason and delta_unfold(env, ctx, *x.reason);
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::deref_t& x)
{
    return delta_unfold(env, ctx, x.ref.get());
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::boolean_expr_t& x)
{
    return match(
        x.value,
        [&] (expr_t::boolean_expr_t::not_t& x) { return delta_unfold(env, ctx, x.expr.get()); },
        [&] (auto& x) { return delta_unfold(env, ctx, x.lhs.get()) or delta_unfold(env, ctx, x.rhs.get()); });
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::relation_expr_t& x)
{
    return match(
        x.value,
        [&] (auto& x) { return delta_unfold(env, ctx, x.lhs.get()) or delta_unfold(env, ctx, x.rhs.get()); });
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::arith_expr_t& x)
{
    return match(
        x.value,
        [&] (auto& x) { return delta_unfold(env, ctx, x.lhs.get()) or delta_unfold(env, ctx, x.rhs.get()); });
}

bool delta_unfold(env_t const&, ctx_t const&, expr_t::global_t&)
{
    // We only perform delta-unfolding inside a direct application,
    // eg `f(x)` for some global function `f`, but not everywhere, eg `return f`.
    // The reason for this is that we are interested in delta-unfolding only as
    // a way to extend beta-normalization as far as possible.
    return false;
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::app_t& app)
{
    if (is_mutable(app))
        // delta-unfolding is only useful to extend beta-normalization as far as possible,
        // but mutable operations cannot be reduced to normal-formal anyway
        return false;
    if (auto const global = std::get_if<expr_t::global_t>(&app.func.get().value))
        if (auto const func_def = std::get_if<func_def_t>(env[*global]))
        {
            app.func.get().value = func_def->value;
            return true;
        }
    if (delta_unfold(env, ctx, app.func.get()))
        return true;
    for (auto& arg: app.args)
        if (delta_unfold(env, ctx, arg))
            return true;
    return false;
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::abs_t& abs)
{
    auto ctx2 = ctx.extend();
    for (auto& arg: abs.args)
    {
        if (delta_unfold(env, ctx2, arg.type))
            return true;
        auto const inserted = ctx2.try_emplace(arg.var, std::nullopt, ctx_t::var_decl_t{arg.qty, arg.type});
        assert(inserted.has_value());
    }
    if (delta_unfold(env, ctx2, abs.ret_type.get()))
        return true;
    return delta_unfold(env, ctx2, abs.body);
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::pi_t& pi)
{
    auto ctx2 = ctx.extend();
    for (auto& arg: pi.args)
    {
        if (delta_unfold(env, ctx2, arg.type))
            return true;
        auto const inserted = ctx2.try_emplace(arg.var, std::nullopt, ctx_t::var_decl_t{arg.qty, arg.type});
        assert(inserted.has_value());
    }
    return delta_unfold(env, ctx2, pi.ret_type.get());
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::sigma_t& sigma)
{
    auto ctx2 = ctx.extend();
    for (auto& arg: sigma.args)
    {
        if (delta_unfold(env, ctx2, arg.type))
            return true;
        auto const inserted = ctx2.try_emplace(arg.var, std::nullopt, ctx_t::var_decl_t{arg.qty, arg.type});
        assert(inserted.has_value());
    }
    return false;
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::init_list_t& init_list)
{
    for (auto& v: init_list.values)
        if (delta_unfold(env, ctx, v))
            return true;
    return false;
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::member_t& subscript)
{
    return delta_unfold(env, ctx, subscript.object.get());
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::subscript_t& subscript)
{
    return delta_unfold(env, ctx, subscript.object.get()) or delta_unfold(env, ctx, subscript.index.get());
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::because_t& x)
{
    return delta_unfold(env, ctx, x.value.get()) or delta_unfold(env, ctx, x.reason.get());
}

cpp_int
reduce(
    env_t const& env,
    hana::type<expr_t::arith_expr_t::plus_t>,
    cpp_int const& a,
    cpp_int const& b,
    expr_t const& type)
{
    return match(
        type.value,
        [&] (expr_t::i8_t) { return cpp_int_add_signed<8>(a, b); },
        [&] (expr_t::i16_t) { return cpp_int_add_signed<16>(a, b); },
        [&] (expr_t::i32_t) { return cpp_int_add_signed<32>(a, b); },
        [&] (expr_t::i64_t) { return cpp_int_add_signed<64>(a, b); },
        [&] (expr_t::u8_t) { return cpp_int_add_unsigned<8>(a, b); },
        [&] (expr_t::u16_t) { return cpp_int_add_unsigned<16>(a, b); },
        [&] (expr_t::u32_t) { return cpp_int_add_unsigned<32>(a, b); },
        [&] (expr_t::u64_t) { return cpp_int_add_unsigned<64>(a, b); },
        [&] (expr_t::global_t const& g)
        {
            auto const type_def = std::get_if<type_def_t>(env[g]);
            assert(type_def and "global must refer to a typedef");
            return match(
                type_def->value,
                [&] (type_def_t::integer_t const& integer)
                {
                    return cpp_int_add(integer.sign, integer.width, a, b);
                },
                [] (type_def_t::struct_t const&)
                {
                    assert(false and "invalid type for primitive delta-reduction of `a + b`");
                    return cpp_int{};
                });
        },
        [&] (auto const&)
        {
            assert(false and "invalid type for primitive delta-reduction of `a + b`");
            return cpp_int{};
        });
}

cpp_int
reduce(
    env_t const& env,
    hana::type<expr_t::arith_expr_t::minus_t>,
    cpp_int const& a,
    cpp_int const& b,
    expr_t const& type)
{
    return match(
        type.value,
        [&] (expr_t::i8_t) { return cpp_int_sub_signed<8>(a, b); },
        [&] (expr_t::i16_t) { return cpp_int_sub_signed<16>(a, b); },
        [&] (expr_t::i32_t) { return cpp_int_sub_signed<32>(a, b); },
        [&] (expr_t::i64_t) { return cpp_int_sub_signed<64>(a, b); },
        [&] (expr_t::u8_t) { return cpp_int_sub_unsigned<8>(a, b); },
        [&] (expr_t::u16_t) { return cpp_int_sub_unsigned<16>(a, b); },
        [&] (expr_t::u32_t) { return cpp_int_sub_unsigned<32>(a, b); },
        [&] (expr_t::u64_t) { return cpp_int_sub_unsigned<64>(a, b); },
        [&] (expr_t::global_t const& g)
        {
            auto const type_def = std::get_if<type_def_t>(env[g]);
            assert(type_def and "global must refer to a typedef");
            return match(
                type_def->value,
                [&] (type_def_t::integer_t const& integer)
                {
                    return cpp_int_sub(integer.sign, integer.width, a, b);
                },
                [] (type_def_t::struct_t const&)
                {
                    assert(false and "invalid type for primitive delta-reduction of `a - b`");
                    return cpp_int{};
                });
        },
        [&] (auto const&)
        {
            assert(false and "invalid type for primitive delta-reduction of `a - b`");
            return cpp_int{};
        });
}

cpp_int
reduce(
    env_t const& env,
    hana::type<expr_t::arith_expr_t::mult_t>,
    cpp_int const& a,
    cpp_int const& b,
    expr_t const& type)
{
    return match(
        type.value,
        [&] (expr_t::i8_t) { return cpp_int_mult_signed<8>(a, b); },
        [&] (expr_t::i16_t) { return cpp_int_mult_signed<16>(a, b); },
        [&] (expr_t::i32_t) { return cpp_int_mult_signed<32>(a, b); },
        [&] (expr_t::i64_t) { return cpp_int_mult_signed<64>(a, b); },
        [&] (expr_t::u8_t) { return cpp_int_mult_unsigned<8>(a, b); },
        [&] (expr_t::u16_t) { return cpp_int_mult_unsigned<16>(a, b); },
        [&] (expr_t::u32_t) { return cpp_int_mult_unsigned<32>(a, b); },
        [&] (expr_t::u64_t) { return cpp_int_mult_unsigned<64>(a, b); },
        [&] (expr_t::global_t const& g)
        {
            auto const type_def = std::get_if<type_def_t>(env[g]);
            assert(type_def and "global must refer to a typedef");
            return match(
                type_def->value,
                [&] (type_def_t::integer_t const& integer)
                {
                    return cpp_int_mult(integer.sign, integer.width, a, b);
                },
                [] (type_def_t::struct_t const&)
                {
                    assert(false and "invalid type for primitive delta-reduction of `a * b`");
                    return cpp_int{};
                });
        },
        [&] (auto const&)
        {
            assert(false and "invalid type for primitive delta-reduction of `a * b`");
            return cpp_int{};
        });
}

cpp_int
reduce(
    env_t const& env,
    hana::type<expr_t::arith_expr_t::div_t>,
    cpp_int const& a,
    cpp_int const& b,
    expr_t const& type)
{
    return match(
        type.value,
        [&] (expr_t::i8_t) { return cpp_int_div_signed<8>(a, b); },
        [&] (expr_t::i16_t) { return cpp_int_div_signed<16>(a, b); },
        [&] (expr_t::i32_t) { return cpp_int_div_signed<32>(a, b); },
        [&] (expr_t::i64_t) { return cpp_int_div_signed<64>(a, b); },
        [&] (expr_t::u8_t) { return cpp_int_div_unsigned<8>(a, b); },
        [&] (expr_t::u16_t) { return cpp_int_div_unsigned<16>(a, b); },
        [&] (expr_t::u32_t) { return cpp_int_div_unsigned<32>(a, b); },
        [&] (expr_t::u64_t) { return cpp_int_div_unsigned<64>(a, b); },
        [&] (expr_t::global_t const& g)
        {
            auto const type_def = std::get_if<type_def_t>(env[g]);
            assert(type_def and "global must refer to a typedef");
            return match(
                type_def->value,
                [&] (type_def_t::integer_t const& integer)
                {
                    return cpp_int_div(integer.sign, integer.width, a, b);
                },
                [] (type_def_t::struct_t const&)
                {
                    assert(false and "invalid type for primitive delta-reduction of `a / b`");
                    return cpp_int{};
                });
        },
        [&] (auto const&)
        {
            assert(false and "invalid type for primitive delta-reduction of `a / b`");
            return cpp_int{};
        });
}

} // namespace impl

bool delta_unfold(env_t const& env, ctx_t const& ctx, body_t& body)
{
    for (auto& s: body.stmts)
        if (impl::delta_unfold(env, ctx, s))
            return true;
    return false;
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t& expr)
{
    if (auto const type = std::get_if<expr_t>(&expr.properties.sort.get()))
        if (delta_unfold(env, ctx, *type))
            return true;
    // for boolean_expr_t, relation_expr_t,  etc, prefer primitive reduction over further unfolding
    return match(
        expr.value,
        [&] (expr_t::boolean_expr_t& x)
        {
            bool const changed = match(
                x.value,
                [&] (expr_t::boolean_expr_t::not_t const& x)
                {
                    if (auto const c = std::get_if<expr_t::boolean_constant_t>(&x.expr.get().value))
                    {
                        bool const value = not c->value; // compute result before destructive self assignment
                        expr.value.template emplace<expr_t::boolean_constant_t>(value);
                        return true;
                    }
                    return false;
                },
                [&] <typename T> (T const& x)
                {
                    if (auto const a = std::get_if<expr_t::boolean_constant_t>(&x.lhs.get().value))
                        if (auto const b = std::get_if<expr_t::boolean_constant_t>(&x.rhs.get().value))
                        {
                            bool const c = impl::reduce(hana::type_c<T>, a->value, b->value);
                            expr.value.template emplace<expr_t::boolean_constant_t>(c);
                            return true;
                        }
                    return false;
                });
            return changed or impl::delta_unfold(env, ctx, x);
        },
        [&] (expr_t::relation_expr_t& x)
        {
            bool const changed = match(
                x.value,
                [&] <typename T> (T const& x)
                {
                    if (auto const a = std::get_if<expr_t::boolean_constant_t>(&x.lhs.get().value))
                        if (auto const b = std::get_if<expr_t::boolean_constant_t>(&x.rhs.get().value))
                        {
                            bool const c = impl::reduce(hana::type_c<T>, a->value, b->value);
                            expr.value.template emplace<expr_t::boolean_constant_t>(c);
                            return true;
                        }
                    if (auto const a = std::get_if<expr_t::numeric_constant_t>(&x.lhs.get().value))
                        if (auto const b = std::get_if<expr_t::numeric_constant_t>(&x.rhs.get().value))
                        {
                            bool const c = impl::reduce(hana::type_c<T>, a->value, b->value);
                            expr.value.template emplace<expr_t::boolean_constant_t>(c);
                            return true;
                        }
                    return false;
                });
            return changed or impl::delta_unfold(env, ctx, x);
        },
        [&] (expr_t::arith_expr_t& x)
        {
            bool const changed = match(
                x.value,
                [&] <typename T> (T const& x)
                {
                    if (auto const n = std::get_if<expr_t::numeric_constant_t>(&x.lhs.get().value))
                        if (auto const m = std::get_if<expr_t::numeric_constant_t>(&x.rhs.get().value))
                        {
                            auto const& ty = std::get<expr_t>(x.lhs.get().properties.sort.get());
                            expr.value = expr_t::numeric_constant_t{
                                impl::reduce(env, hana::type_c<T>, n->value, m->value, ty)
                            };
                            return true;
                        }
                    return false;
                });
            return changed or impl::delta_unfold(env, ctx, x);
        },
        [&] (expr_t::member_t& member)
        {
            bool changed = false;
            if (auto const init_list = std::get_if<expr_t::init_list_t>(&member.object.get().value))
                if (auto const type = std::get_if<expr_t>(&member.object.get().properties.sort.get()))
                    if (auto const g = std::get_if<expr_t::global_t>(&type->value))
                        if (auto const type_def = std::get_if<type_def_t>(env[*g]))
                            if (auto const s = std::get_if<type_def_t::struct_t>(&type_def->value))
                                if (auto const i = ast::find_member_index<properties_t>(member.field, *s))
                                {
                                    changed = true;
                                    destructive_self_assign(expr.value, std::move(init_list->values[*i].value));
                                }
            return changed or impl::delta_unfold(env, ctx, member);
        },
        [&] (expr_t::subscript_t& subscript)
        {
            bool changed = false;
            if (auto const init_list = std::get_if<expr_t::init_list_t>(&subscript.object.get().value))
                if (auto const i = std::get_if<expr_t::numeric_constant_t>(&subscript.index.get().value))
                    if (i->value <= std::numeric_limits<std::size_t>::max())
                    {
                        changed = true;
                        auto const i_ = i->value.template convert_to<std::size_t>();
                        destructive_self_assign(expr.value, std::move(init_list->values[i_].value));
                    }
            return changed or impl::delta_unfold(env, ctx, subscript);
        },
        [&] (expr_t::app_t& app)
        {
            bool const changed = match(
                is_builtin_call(app),
                [] (is_builtin_call_result::no_t) { return false; },
                [&] (is_builtin_call_result::slice_t const& slice)
                {
                    // `slice` is a view into `app`, which is non-const, so it's ok to const_cast away
                    auto const l = const_cast<expr_t::init_list_t*>(std::get_if<expr_t::init_list_t>(&slice.xs.value));
                    auto const k = std::get_if<expr_t::numeric_constant_t>(&slice.k.value);
                    if (l and k)
                    {
                        l->values.erase(l->values.begin(), l->values.begin() + k->value.template convert_to<std::size_t>());
                        // note this is not destructive self-assignment because we first construct a new value_t
                        expr.value = expr_t::value_t{std::move(*l)};
                    }
                    return l and k;
                });
            return changed or impl::delta_unfold(env, ctx, app);
        },
        [&] (auto& x) { return impl::delta_unfold(env, ctx, x); });
}

} // namespace dep0::typecheck
