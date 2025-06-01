/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/derivation_rules.hpp"

namespace dep0::typecheck {

expr_t derivation_rules::make_typename() { return make_legal_expr(kind_t{}, expr_t::typename_t{}); }
expr_t derivation_rules::make_true_t()
{
    return make_legal_expr(
        make_legal_expr(
            kind_t{}, // TODO need to add a test to make sure this is correct
            expr_t::pi_t{
                ast::is_mutable_t::no,
                std::vector{make_legal_func_arg(ast::qty_t::zero, make_bool())},
                make_typename()}),
        expr_t::true_t{});
}
expr_t derivation_rules::make_true_t(expr_t cond)
{
    return make_app(make_true_t(), {std::move(cond)});
}

expr_t derivation_rules::make_addressof(expr_t type, expr_t::scopeof_t scope)
{
    auto expr = scope.expr;
    return make_legal_expr(
        make_app(make_ref_t(), {std::move(type), make_legal_expr(make_scope_t(), std::move(scope))}),
        expr_t::addressof_t{std::move(expr)});
}

expr_t derivation_rules::make_ref_t()
{
    return make_legal_expr(
        make_legal_expr(
            kind_t{},
            expr_t::pi_t{
                ast::is_mutable_t::no,
                std::vector{
                    make_legal_func_arg(ast::qty_t::zero, make_typename()),
                    make_legal_func_arg(ast::qty_t::zero, make_scope_t())
                },
                make_typename()}),
        expr_t::ref_t{});
}
expr_t derivation_rules::make_scope_t() { return make_legal_expr(make_typename(), expr_t::scope_t{}); }
expr_t derivation_rules::make_bool() { return make_legal_expr(make_typename(), expr_t::bool_t{}); }
expr_t derivation_rules::make_cstr() { return make_legal_expr(make_typename(), expr_t::cstr_t{}); }
expr_t derivation_rules::make_unit() { return make_legal_expr(make_typename(), expr_t::unit_t{}); }
expr_t derivation_rules::make_i8() { return make_legal_expr(make_typename(), expr_t::i8_t{}); }
expr_t derivation_rules::make_i16() { return make_legal_expr(make_typename(), expr_t::i16_t{}); }
expr_t derivation_rules::make_i32() { return make_legal_expr(make_typename(), expr_t::i32_t{}); }
expr_t derivation_rules::make_i64() { return make_legal_expr(make_typename(), expr_t::i64_t{}); }
expr_t derivation_rules::make_u8() { return make_legal_expr(make_typename(), expr_t::u8_t{}); }
expr_t derivation_rules::make_u16() { return make_legal_expr(make_typename(), expr_t::u16_t{}); }
expr_t derivation_rules::make_u32() { return make_legal_expr(make_typename(), expr_t::u32_t{}); }
expr_t derivation_rules::make_u64() { return make_legal_expr(make_typename(), expr_t::u64_t{}); }

expr_t derivation_rules::make_u64(std::size_t const v)
{
    return make_legal_expr(make_u64(), expr_t::numeric_constant_t{v});
}

expr_t derivation_rules::make_true() { return make_legal_expr(make_bool(), expr_t::boolean_constant_t{true}); }
expr_t derivation_rules::make_false() { return make_legal_expr(make_bool(), expr_t::boolean_constant_t{false}); }

expr_t derivation_rules::make_boolean_expr(expr_t::boolean_expr_t::value_t v)
{
    return make_legal_expr(make_bool(), expr_t::boolean_expr_t{std::move(v)});
}

expr_t derivation_rules::make_relation_expr(expr_t::relation_expr_t::value_t v)
{
    return make_legal_expr(make_bool(), expr_t::relation_expr_t{std::move(v)});
}

expr_t derivation_rules::make_array()
{
    // the elemnt type of the array must be of sort types,
    // i.e. a "function argument" of type `typename`;
    // to have array of types (eg `{int, bool, bool}`),
    // we fisrt need to add cumulativity of types;
    // i.e. we can have an array of ints, eg {1,2,3},
    // but not an array of types, eg {int, bool, bool};
    // with cumulativity we can have both
    return make_legal_expr(
        make_legal_expr(
            kind_t{}, // TODO need to add a test to make sure this is correct
            expr_t::pi_t{
                ast::is_mutable_t::no,
                std::vector{
                    make_legal_func_arg(ast::qty_t::zero, make_typename()),
                    make_legal_func_arg(ast::qty_t::zero, make_u64())
                },
                make_typename()}),
        expr_t::array_t{});
}

expr_t derivation_rules::make_app(expr_t func, std::vector<expr_t> args)
{
    auto const func_type = std::get_if<expr_t>(&func.properties.sort.get());
    assert(func_type);
    auto const pi_type = std::get_if<expr_t::pi_t>(&func_type->value);
    assert(pi_type);
    auto ret_type = pi_type->ret_type.get(); // make copy before moving func
    return make_legal_expr(std::move(ret_type), expr_t::app_t{std::move(func), std::move(args)});
}

expr_t derivation_rules::make_subscript(expr_t obj, std::size_t const idx, sort_t type)
{
    return make_legal_expr(std::move(type), expr_t::subscript_t{std::move(obj), derivation_rules::make_u64(idx)});
}

} // namespace dep0::typecheck

