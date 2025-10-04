/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/derivation_rules.hpp"

namespace dep0::typecheck {

expr_t derivation_rules::make_typename(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, kind_t{}, expr_t::typename_t{});
}
expr_t derivation_rules::make_true_t(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(
        env, ctx,
        make_legal_expr(
            env, ctx,
            kind_t{}, // TODO need to add a test to make sure this is correct
            expr_t::pi_t{
                ast::is_mutable_t::no,
                std::vector{make_legal_func_arg(ast::qty_t::zero, make_bool(env, ctx))},
                make_typename(env, ctx)}),
        expr_t::true_t{});
}
expr_t derivation_rules::make_true_t(env_t const& env, ctx_t const& ctx, expr_t cond)
{
    return make_app(env, ctx, make_true_t(env, ctx), {std::move(cond)});
}

expr_t
derivation_rules::make_addressof(
    env_t const& env, ctx_t const& ctx, expr_t element_type, expr_t::scopeof_t scope, expr_t expr)
{
    return make_legal_expr(
        env, ctx,
        make_app(
            env, ctx,
            make_ref_t(env, ctx),
            {std::move(element_type), make_legal_expr(env, ctx, make_scope_t(env, ctx), std::move(scope))}),
        expr_t::addressof_t{std::move(expr)});
}

expr_t derivation_rules::make_ref_t(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(
        env, ctx,
        make_legal_expr(
            env, ctx,
            kind_t{},
            expr_t::pi_t{
                ast::is_mutable_t::no,
                std::vector{
                    make_legal_func_arg(ast::qty_t::zero, make_typename(env, ctx)),
                    make_legal_func_arg(ast::qty_t::zero, make_scope_t(env, ctx))
                },
                make_typename(env, ctx)}),
        expr_t::ref_t{});
}
expr_t derivation_rules::make_scope_t(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, make_typename(env, ctx), expr_t::scope_t{});
}

expr_t derivation_rules::make_bool(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, make_typename(env, ctx), expr_t::bool_t{});
}

expr_t derivation_rules::make_cstr(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, make_typename(env, ctx), expr_t::cstr_t{});
}

expr_t derivation_rules::make_unit(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, make_typename(env, ctx), expr_t::unit_t{});
}

expr_t derivation_rules::make_i8(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, make_typename(env, ctx), expr_t::i8_t{});
}

expr_t derivation_rules::make_i16(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, make_typename(env, ctx), expr_t::i16_t{});
}

expr_t derivation_rules::make_i32(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, make_typename(env, ctx), expr_t::i32_t{});
}

expr_t derivation_rules::make_i64(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, make_typename(env, ctx), expr_t::i64_t{});
}

expr_t derivation_rules::make_u8(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, make_typename(env, ctx), expr_t::u8_t{});
}

expr_t derivation_rules::make_u16(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, make_typename(env, ctx), expr_t::u16_t{});
}

expr_t derivation_rules::make_u32(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, make_typename(env, ctx), expr_t::u32_t{});
}

expr_t derivation_rules::make_u64(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, make_typename(env, ctx), expr_t::u64_t{});
}

expr_t derivation_rules::make_u64(env_t const& env, ctx_t const& ctx, std::size_t const v)
{
    return make_legal_expr(env, ctx, make_u64(env, ctx), expr_t::numeric_constant_t{v});
}

expr_t derivation_rules::make_true(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, make_bool(env, ctx), expr_t::boolean_constant_t{true});
}

expr_t derivation_rules::make_false(env_t const& env, ctx_t const& ctx)
{
    return make_legal_expr(env, ctx, make_bool(env, ctx), expr_t::boolean_constant_t{false});
}

expr_t derivation_rules::make_boolean_expr(env_t const& env, ctx_t const& ctx, expr_t::boolean_expr_t::value_t v)
{
    return make_legal_expr(env, ctx, make_bool(env, ctx), expr_t::boolean_expr_t{std::move(v)});
}

expr_t derivation_rules::make_relation_expr(env_t const& env, ctx_t const& ctx, expr_t::relation_expr_t::value_t v)
{
    return make_legal_expr(env, ctx, make_bool(env, ctx), expr_t::relation_expr_t{std::move(v)});
}

expr_t derivation_rules::make_array(env_t const& env, ctx_t const& ctx)
{
    // the elemnt type of the array must be of sort types,
    // i.e. a "function argument" of type `typename`;
    // to have array of types (eg `{int, bool, bool}`),
    // we fisrt need to add cumulativity of types;
    // i.e. we can have an array of ints, eg {1,2,3},
    // but not an array of types, eg {int, bool, bool};
    // with cumulativity we can have both
    return make_legal_expr(
        env, ctx,
        make_legal_expr(
            env, ctx,
            kind_t{}, // TODO need to add a test to make sure this is correct
            expr_t::pi_t{
                ast::is_mutable_t::no,
                std::vector{
                    make_legal_func_arg(ast::qty_t::zero, make_typename(env, ctx)),
                    make_legal_func_arg(ast::qty_t::zero, make_u64(env, ctx))
                },
                make_typename(env, ctx)}),
        expr_t::array_t{});
}

expr_t derivation_rules::make_app(env_t const& env, ctx_t const& ctx, expr_t func, std::vector<expr_t> args)
{
    auto const func_type = std::get_if<expr_t>(&func.properties.sort.get());
    assert(func_type);
    auto const pi_type = std::get_if<expr_t::pi_t>(&func_type->value);
    assert(pi_type);
    auto ret_type = pi_type->ret_type.get(); // make copy before moving func
    return make_legal_expr(env, ctx, std::move(ret_type), expr_t::app_t{std::move(func), std::move(args)});
}

expr_t
derivation_rules::make_subscript(env_t const& env, ctx_t const& ctx, expr_t obj, std::size_t const idx, sort_t type)
{
    return make_legal_expr(
        env, ctx, std::move(type), expr_t::subscript_t{std::move(obj), derivation_rules::make_u64(env, ctx, idx)});
}

} // namespace dep0::typecheck
