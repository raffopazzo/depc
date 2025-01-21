/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Implementation details of pretty_print.hpp
 */
#pragma once

#include "dep0/ast/pretty_print.hpp"

#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::ast {

namespace detail {

/** @brief Test whether a call to `dep0::ast::pretty_print()` would print a new line. */
template <Properties P> bool needs_new_line(body_t<P> const&);
template <Properties P> bool needs_new_line(func_arg_t<P> const&);
template <Properties P> bool needs_new_line(expr_t<P> const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::typename_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::true_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::auto_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::bool_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::cstr_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::unit_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::i8_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::i16_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::i32_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::i64_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::u8_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::u16_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::u32_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::u64_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::boolean_constant_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::numeric_constant_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::string_literal_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::boolean_expr_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::relation_expr_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::arith_expr_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::var_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::global_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::app_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::abs_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::pi_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::sigma_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::array_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::init_list_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::subscript_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::because_t const&);

/**
 * @brief Test whether an expression should be printed inside parenthesis,
 * for example `(x + y)` but not `x[1][2]` nor `f(x+y)()`.
 */
template <Properties P> bool needs_parenthesis(expr_t<P> const&);
template <Properties P> bool needs_parenthesis(typename expr_t<P>::typename_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::true_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::auto_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::bool_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::cstr_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::unit_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::i8_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::i16_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::i32_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::i64_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::u8_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::u16_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::u32_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::u64_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::boolean_constant_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::numeric_constant_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::string_literal_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::boolean_expr_t const&);
template <Properties P> bool needs_parenthesis(typename expr_t<P>::relation_expr_t const&) { return true; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::arith_expr_t const&) { return true; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::var_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::global_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::app_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::abs_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::pi_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::sigma_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::array_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::init_list_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::subscript_t const&) { return false; }
template <Properties P> bool needs_parenthesis(typename expr_t<P>::because_t const&) { return false; }

/** @brief Prints a new line followed by the correct number of white spaces for the given indentation level. */
inline std::ostream& new_line(std::ostream& os, std::size_t const indent)
{
    return os << std::endl << std::string(indent * 4ul, ' ');
}

/** @brief Prints the given expression inside parenthesis if required, otherwise without. */
template <Properties P>
std::ostream& maybe_with_paranthesis(std::ostream& os, expr_t<P> const& x, std::size_t const indent)
{
    return detail::needs_parenthesis(x)
        ? pretty_print(os << '(', x, indent) << ')'
        : pretty_print(os, x, indent);
}

} // namespace detail

template <Properties P>
std::ostream& pretty_print(std::ostream& os, module_t<P> const& module, std::size_t const indent)
{
    bool first = true;
    for (auto const& x: module.entries)
        match(
            x,
            [&] (auto const& x)
            {
                pretty_print(std::exchange(first, false) ? os : detail::new_line(os, indent), x, indent);
            });
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, type_def_t<P> const& type_def, std::size_t)
{
    match(
        type_def.value,
        [&] (typename type_def_t<P>::integer_t const& x)
        {
            os << "typedef " << x.name << " = ";
            os << (x.sign == sign_t::signed_v ? "signed" : "unsigned") << ' ';
            os << (
                x.width == width_t::_8 ? "8" :
                x.width == width_t::_16 ? "16" :
                x.width == width_t::_32 ? "32" :
                "64") << " bit integer;";
        });
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, axiom_t<P> const& axiom, std::size_t const indent)
{
    return pretty_print<P>(os << "axiom " << axiom.name, axiom.signature) << ';';
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, extern_decl_t<P> const& extern_decl, std::size_t const indent)
{
    return pretty_print<P>(os << "extern " << extern_decl.name, extern_decl.signature) << ';';
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, func_decl_t<P> const& func_decl, std::size_t const indent)
{
    os << "func " << func_decl.name;
    if (func_decl.attribute)
        os << "[[" << func_decl.attribute->value << "]] ";
    return pretty_print<P>(os, func_decl.signature) << ';';
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, func_def_t<P> const& func_def, std::size_t const indent)
{
    os << "func " << func_def.name;
    if (func_def.attribute)
        os << "[[" << func_def.attribute->value << "]] ";
    auto const& abs = func_def.value;
    pretty_print(os, abs.is_mutable, abs.args.begin(), abs.args.end(), abs.ret_type.get(), indent);
    detail::new_line(os, indent);
    pretty_print(os, abs.body, indent);
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, func_arg_t<P> const& x, std::size_t const indent)
{
    if (x.qty == ast::qty_t::zero)
        os << "0 ";
    else if (x.qty == ast::qty_t::one)
        os << "1 ";
    pretty_print(os, x.type, indent);
    if (x.var)
        pretty_print<P>(os << ' ', *x.var, indent);
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, body_t<P> const& x, std::size_t const indent)
{
    if (x.stmts.empty())
        return os << "{ }";
    os << '{';
    for (auto const& s: x.stmts)
        pretty_print(detail::new_line(os, indent + 1ul), s, indent + 1ul);
    detail::new_line(os, indent) << '}';
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, stmt_t<P> const& x, std::size_t const indent)
{
    match(x.value, [&] (auto const& x) { pretty_print<P>(os, x, indent); });
    if (std::holds_alternative<typename expr_t<P>::app_t>(x.value))
        os << ';';
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename stmt_t<P>::if_else_t const& x, std::size_t const indent)
{
    auto const print_stmt_or_body = [&] (body_t<P> const& body)
    {
        if (body.stmts.size() == 1ul and not std::holds_alternative<typename stmt_t<P>::if_else_t>(body.stmts[0].value))
            pretty_print(detail::new_line(os, indent + 1ul), body.stmts[0], indent + 1ul);
        else
            pretty_print(detail::new_line(os, indent), body, indent);
    };
    pretty_print(os << "if (", x.cond, indent + 1ul) << ')';
    print_stmt_or_body(x.true_branch);
    if (x.false_branch)
    {
        detail::new_line(os, indent) << "else";
        print_stmt_or_body(*x.false_branch);
    }
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename stmt_t<P>::return_t const& x, std::size_t const indent)
{
    if (x.expr)
        pretty_print(os << "return ", *x.expr, indent + 1ul) << ';';
    else
        os << "return;";
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename stmt_t<P>::impossible_t const& x, std::size_t const indent)
{
    os << "impossible";
    if (x.reason)
        pretty_print(os << " because ", *x.reason);
    return os << ';';
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, expr_t<P> const& x, std::size_t const indent)
{
    match(x.value, [&] (auto const& x) { pretty_print<P>(os, x, indent); });
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::typename_t const&, std::size_t)
{
    return os << "typename";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::true_t const&, std::size_t)
{
    return os << "true_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::auto_t const&, std::size_t)
{
    return os << "auto";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::bool_t const&, std::size_t)
{
    return os << "bool_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::cstr_t const&, std::size_t)
{
    return os << "cstr_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::unit_t const&, std::size_t)
{
    return os << "unit_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::i8_t const&, std::size_t)
{
    return os << "i8_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::i16_t const&, std::size_t)
{
    return os << "i16_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::i32_t const&, std::size_t)
{
    return os << "i32_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::i64_t const&, std::size_t)
{
    return os << "i64_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::u8_t const&, std::size_t)
{
    return os << "u8_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::u16_t const&, std::size_t)
{
    return os << "u16_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::u32_t const&, std::size_t)
{
    return os << "u32_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::u64_t const&, std::size_t)
{
    return os << "u64_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::boolean_constant_t const& x, std::size_t const indent)
{
    return os << (x.value ? "true" : "false");
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::numeric_constant_t const& x, std::size_t const indent)
{
    return os << x.value;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::string_literal_t const& x, std::size_t const indent)
{
    return os << '"' << x.value << '"';
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::boolean_expr_t const& x, std::size_t const indent)
{
    match(
        x.value,
        [&] (typename expr_t<P>::boolean_expr_t::not_t const& x)
        {
            detail::maybe_with_paranthesis(os << "not ", x.expr.get(), indent);
        },
        [&] (typename expr_t<P>::boolean_expr_t::and_t const& x)
        {
            detail::maybe_with_paranthesis(os, x.lhs.get(), indent);
            detail::maybe_with_paranthesis(os << " and ", x.rhs.get(), indent);
        },
        [&] (typename expr_t<P>::boolean_expr_t::or_t const& x)
        {
            detail::maybe_with_paranthesis(os, x.lhs.get(), indent);
            detail::maybe_with_paranthesis(os << " or ", x.rhs.get(), indent);
        });
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::relation_expr_t const& x, std::size_t const indent)
{
    match(
        x.value,
        [&] (typename expr_t<P>::relation_expr_t::eq_t const& x)
        {
            detail::maybe_with_paranthesis(os, x.lhs.get(), indent);
            detail::maybe_with_paranthesis(os << " == ", x.rhs.get(), indent);
        },
        [&] (typename expr_t<P>::relation_expr_t::neq_t const& x)
        {
            detail::maybe_with_paranthesis(os, x.lhs.get(), indent);
            detail::maybe_with_paranthesis(os << " != ", x.rhs.get(), indent);
        },
        [&] (typename expr_t<P>::relation_expr_t::gt_t const& x)
        {
            detail::maybe_with_paranthesis(os, x.lhs.get(), indent);
            detail::maybe_with_paranthesis(os << " > ", x.rhs.get(), indent);
        },
        [&] (typename expr_t<P>::relation_expr_t::gte_t const& x)
        {
            detail::maybe_with_paranthesis(os, x.lhs.get(), indent);
            detail::maybe_with_paranthesis(os << " >= ", x.rhs.get(), indent);
        },
        [&] (typename expr_t<P>::relation_expr_t::lt_t const& x)
        {
            detail::maybe_with_paranthesis(os, x.lhs.get(), indent);
            detail::maybe_with_paranthesis(os << " < ", x.rhs.get(), indent);
        },
        [&] (typename expr_t<P>::relation_expr_t::lte_t const& x)
        {
            detail::maybe_with_paranthesis(os, x.lhs.get(), indent);
            detail::maybe_with_paranthesis(os << " <= ", x.rhs.get(), indent);
        });
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::arith_expr_t const& x, std::size_t const indent)
{
    match(
        x.value,
        [&] (typename expr_t<P>::arith_expr_t::plus_t const& x)
        {
            pretty_print(os, x.lhs.get(), indent);
            pretty_print(os << " + ", x.rhs.get(), indent);
        },
        [&] (typename expr_t<P>::arith_expr_t::minus_t const& x)
        {
            pretty_print(os, x.lhs.get(), indent);
            pretty_print(os << " - ", x.rhs.get(), indent);
        },
        [&] (typename expr_t<P>::arith_expr_t::mult_t const& x)
        {
            pretty_print(os, x.lhs.get(), indent);
            pretty_print(os << " * ", x.rhs.get(), indent);
        },
        [&] (typename expr_t<P>::arith_expr_t::div_t const& x)
        {
            pretty_print(os, x.lhs.get(), indent);
            pretty_print(os << " / ", x.rhs.get(), indent);
        });
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::var_t const& x, std::size_t)
{
    os << x.name;
    if (x.idx)
        os << ':' << x.idx;
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::global_t const& x, std::size_t)
{
    if (x.module_name)
        os << *x.module_name << "::";
    return os << x.name;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::app_t const& x, std::size_t const indent)
{
    pretty_print(os, x.func.get(), indent) << '(';
    bool const args_on_separate_lines =
        std::any_of(x.args.begin(), x.args.end(), [] (expr_t<P> const& arg) { return detail::needs_new_line(arg); });
    if (args_on_separate_lines)
        detail::new_line(os, indent + 1ul);
    for (bool first = true; auto const& arg: x.args)
    {
        if (not std::exchange(first, false))
        {
            if (args_on_separate_lines)
                detail::new_line(os << ',', indent + 1ul);
            else
                os << ", ";
        }
        pretty_print(os, arg, indent + 1ul);
    }
    return (args_on_separate_lines ? detail::new_line(os, indent) : os) << ')';
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::abs_t const& x, std::size_t const indent)
{
    os << "[] ";
    pretty_print(os, x.is_mutable, x.args.begin(), x.args.end(), x.ret_type.get(), indent);
    detail::new_line(os, indent);
    pretty_print(os, x.body, indent);
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::pi_t const& x, std::size_t const indent)
{
    return pretty_print(os, x.is_mutable, x.args.begin(), x.args.end(), x.ret_type.get(), indent);
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::sigma_t const& x, std::size_t const indent)
{
    bool const args_on_separate_lines =
        std::ranges::any_of(x.args, [] (func_arg_t<P> const& arg) { return detail::needs_new_line(arg); });
    os << '(';
    if (args_on_separate_lines)
        detail::new_line(os, indent + 1ul);
    for (bool first = true; auto const& arg: x.args)
    {
        if (not std::exchange(first, false))
        {
            if (args_on_separate_lines)
                detail::new_line(os << ',', indent + 1ul);
            else
                os << ", ";
        }
        pretty_print(os, arg, indent + 1ul);
    }
    return (args_on_separate_lines ? detail::new_line(os, indent) : os) << ')';
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::array_t const&, std::size_t)
{
    return os << "array_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::init_list_t const& x, std::size_t const indent)
{
    bool const use_separate_lines = detail::needs_new_line<P>(x);
    os << '{';
    if (use_separate_lines)
        detail::new_line(os, indent + 1ul);
    for (bool first = true; auto const& v: x.values)
    {
        if (not std::exchange(first, false))
        {
            if (use_separate_lines)
                detail::new_line(os << ',', indent + 1ul);
            else
                os << ", ";
        }
        pretty_print(os, v, indent + 1ul);
    }
    return (use_separate_lines ? detail::new_line(os, indent) : os) << '}';
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::subscript_t const& x, std::size_t const indent)
{
    pretty_print(os, x.array.get(), indent) << '[';
    pretty_print(os, x.index.get(), indent) << ']';
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::because_t const& x, std::size_t const indent)
{
    pretty_print(os, x.value.get(), indent) << " because ";
    pretty_print(os, x.reason.get(), indent);
    return os;
}

template <Properties P>
std::ostream& pretty_print(
    std::ostream& os,
    is_mutable_t const is_mutable,
    typename std::vector<func_arg_t<P>>::const_iterator const begin,
    typename std::vector<func_arg_t<P>>::const_iterator const end,
    expr_t<P> const& ret_type,
    std::size_t const indent)
{
    bool const args_on_separate_lines =
        std::any_of(begin, end, [] (func_arg_t<P> const& arg) { return detail::needs_new_line(arg); });
    os << '(';
    if (args_on_separate_lines)
        detail::new_line(os, indent + 1ul);
    for (bool first = true; auto const& arg: std::ranges::subrange(begin, end))
    {
        if (not std::exchange(first, false))
        {
            if (args_on_separate_lines)
                detail::new_line(os << ',', indent + 1ul);
            else
                os << ", ";
        }
        pretty_print(os, arg, indent + 1ul);
    }
    if (args_on_separate_lines)
        detail::new_line(os, indent) << (is_mutable == is_mutable_t::yes ? ") mutable -> " : ") -> ");
    else
    {
        os << ')';
        if (is_mutable == is_mutable_t::yes)
            os << " mutable ";
        (detail::needs_new_line(ret_type) ? detail::new_line(os, indent + 1ul) : os) << " -> ";
    }
    pretty_print(os, ret_type, indent + 1ul); // +1 to help distinguish return type and body
    return os;
}

// implementation of needs_new_line
namespace detail {

template <Properties P>
bool needs_new_line(body_t<P> const& x)
{
    return not x.stmts.empty();
}

template <Properties P>
bool needs_new_line(func_arg_t<P> const& x)
{
    return needs_new_line(x.type);
}

template <Properties P>
bool needs_new_line(expr_t<P> const& x)
{
    return match(x.value, [&] (auto const& x) { return needs_new_line<P>(x); });
}

template <Properties P>
bool needs_new_line(typename expr_t<P>::boolean_expr_t const& x)
{
    return match(
        x.value,
        [] (typename expr_t<P>::boolean_expr_t::not_t const& x)
        {
            return needs_new_line(x.expr.get());
        },
        [] (auto const& x)
        {
            return needs_new_line(x.lhs.get()) or needs_new_line(x.rhs.get());
        });
}

template <Properties P>
bool needs_new_line(typename expr_t<P>::relation_expr_t const& x)
{
    return match(
        x.value,
        [] (auto const& x)
        {
            return needs_new_line(x.lhs.get()) or needs_new_line(x.rhs.get());
        });
}

template <Properties P>
bool needs_new_line(typename expr_t<P>::arith_expr_t const& x)
{
    return match(
        x.value,
        [] (auto const& x)
        {
            return needs_new_line(x.lhs.get()) or needs_new_line(x.rhs.get());
        });
}

template <Properties P>
bool needs_new_line(typename expr_t<P>::app_t const& x)
{
    return needs_new_line(x.func.get())
        or std::any_of(x.args.begin(), x.args.end(), [] (expr_t<P> const& arg) { return needs_new_line(arg); });
}

template <Properties P>
bool needs_new_line(typename expr_t<P>::abs_t const& x)
{
    return std::any_of(x.args.begin(), x.args.end(), [] (func_arg_t<P> const& arg) { return needs_new_line(arg); })
        or needs_new_line(x.ret_type.get())
        or needs_new_line(x.body);
}

template <Properties P>
bool needs_new_line(typename expr_t<P>::pi_t const& x)
{
    return needs_new_line(x.ret_type.get())
        or std::any_of(x.args.begin(), x.args.end(), [] (func_arg_t<P> const& arg) { return needs_new_line(arg); });
}

template <Properties P>
bool needs_new_line(typename expr_t<P>::sigma_t const& x)
{
    return std::any_of(x.args.begin(), x.args.end(), [] (func_arg_t<P> const& arg) { return needs_new_line(arg); });
}

template <Properties P>
bool needs_new_line(typename expr_t<P>::init_list_t const& x)
{
    return std::any_of(x.values.begin(), x.values.end(), [] (expr_t<P> const& y) { return needs_new_line(y); });
}

template <Properties P>
bool needs_new_line(typename expr_t<P>::subscript_t const& x)
{
    return needs_new_line(x.array.get()) or needs_new_line(x.index.get());
}

template <Properties P>
bool needs_new_line(typename expr_t<P>::because_t const& x)
{
    return needs_new_line(x.value.get()) or needs_new_line(x.reason.get());
}

} // namespace detail

// implementation of needs_parenthesis
namespace detail {

template <Properties P>
bool needs_parenthesis(expr_t<P> const& x)
{
    return match(x.value, [&] (auto const& x) { return needs_parenthesis<P>(x); });
}

template <Properties P> bool needs_parenthesis(typename expr_t<P>::boolean_expr_t const& x)
{
    return not std::holds_alternative<typename expr_t<P>::boolean_expr_t::not_t>(x.value);
}

} // namespace detail

} // namespace dep0::ast
