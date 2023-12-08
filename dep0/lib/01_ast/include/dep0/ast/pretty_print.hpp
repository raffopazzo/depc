#pragma once

#include "dep0/ast/ast.hpp"

#include "dep0/match.hpp"

#include <ostream>
#include <algorithm>

namespace dep0::ast {

// The general rule across all `pretty_print` functions is that they start writing from the current stream position;
// if they need to go to a new line, they write the new line character and the correct number of indentation spaces,
// thereby moving the stream to the "correct" position for the next call to `pretty_print()`.

// forward declarations

template <Properties P>
std::ostream& pretty_print(std::ostream&, module_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, type_def_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, func_def_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, func_arg_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, body_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, stmt_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename stmt_t<P>::if_else_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename stmt_t<P>::return_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, expr_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::typename_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::bool_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::unit_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::i8_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::i16_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::i32_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::i64_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::u8_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::u16_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::u32_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::u64_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::arith_expr_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::boolean_constant_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::numeric_constant_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::var_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::app_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::abs_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::pi_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(
    std::ostream&,
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    expr_t<P> const& ret_type,
    std::size_t indent = 0ul);

// implementations

inline std::ostream& new_line(std::ostream& os, std::size_t indent)
{
    os << std::endl;
    while (indent--)
        os << "    ";
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, module_t<P> const& module, std::size_t const indent)
{
    bool first = true;
    for (auto const& type_def: module.type_defs)
        pretty_print(std::exchange(first, false) ? os : new_line(os, indent), type_def, indent);
    for (auto const& func_def: module.func_defs)
        pretty_print(std::exchange(first, false) ? os : new_line(os, indent), func_def, indent);
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
                "64") << " bit integer from ";
            if (x.sign == sign_t::unsigned_v)
            {
                os << "0 to ";
                x.max_abs_value ? os << *x.max_abs_value : os << "...";
            }
            else if (x.max_abs_value)
                os << '-' << *x.max_abs_value << " to " << *x.max_abs_value;
            else
                os << "... to ...";
            os << ';';
        });
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, func_def_t<P> const& func_def, std::size_t const indent)
{
    os << "auto " << func_def.name;
    pretty_print(os, func_def.value.args.begin(), func_def.value.args.end(), func_def.value.ret_type.get(), indent);
    new_line(os, indent);
    pretty_print(os, func_def.value.body, indent);
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, func_arg_t<P> const& x, std::size_t const indent)
{
    pretty_print(os, x.type, indent);
    if (x.var)
        pretty_print<P>(os << ' ', *x.var, indent);
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::typename_t const&, std::size_t)
{
    return os << "typename";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::bool_t const&, std::size_t)
{
    return os << "bool";
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
std::ostream& pretty_print(std::ostream& os, body_t<P> const& x, std::size_t const indent)
{
    if (x.stmts.empty())
        return os << "{ }";
    os << '{';
    for (auto const& s: x.stmts)
        pretty_print(new_line(os, indent + 1ul), s, indent + 1ul);
    new_line(os, indent) << '}';
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
            pretty_print(new_line(os, indent + 1ul), body.stmts[0]);
        else
            pretty_print(new_line(os, indent + 1ul), body);
    };
    pretty_print(os << "if (", x.cond, indent + 1ul) << ')';
    print_stmt_or_body(x.true_branch);
    if (x.false_branch)
    {
        new_line(os, indent) << "else";
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
std::ostream& pretty_print(std::ostream& os, expr_t<P> const& x, std::size_t const indent)
{
    match(x.value, [&] (auto const& x) { pretty_print<P>(os, x, indent); });
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
            os << " + ";
            pretty_print(os, x.rhs.get(), indent);
        });
    return os;
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
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::var_t const& x, std::size_t const indent)
{
    os << x.name;
    if (x.idx)
        os << ':' << x.idx;
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::app_t const& x, std::size_t const indent)
{
    pretty_print(os, x.func.get(), indent) << '(';
    for (bool first = true; auto const& arg: x.args)
        pretty_print(std::exchange(first, false) ? os : os << ", ", arg, indent + 1ul);
    os << ')';
    return os;
}


template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::abs_t const& x, std::size_t const indent)
{
    os << "[] ";
    pretty_print(os, x.args.begin(), x.args.end(), x.ret_type.get(), indent);
    new_line(os, indent);
    pretty_print(os, x.body, indent);
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::pi_t const& x, std::size_t const indent)
{
    return pretty_print(os, x.args.begin(), x.args.end(), x.ret_type.get(), indent);
}

template <Properties P>
std::ostream& pretty_print(
    std::ostream& os,
    typename std::vector<func_arg_t<P>>::const_iterator const begin,
    typename std::vector<func_arg_t<P>>::const_iterator const end,
    expr_t<P> const& ret_type,
    std::size_t const indent)
{
    bool const args_on_separate_lines =
        std::distance(begin, end) > 1ul and
        std::any_of(
            begin, end,
            [] (func_arg_t<P> const& arg)
            {
                return std::holds_alternative<typename expr_t<P>::pi_t>(arg.type.value);
            });
    os << '(';
    if (args_on_separate_lines)
        new_line(os, indent + 1ul);
    for (bool first = true; auto const& arg: std::ranges::subrange(begin, end))
    {
        if (not std::exchange(first, false))
        {
            if (args_on_separate_lines)
                new_line(os << ',', indent + 1ul);
            else
                os << ", ";
        }
        pretty_print(os, arg, indent + 1ul);
    }
    (args_on_separate_lines ? new_line(os, indent) : os) << ')';
    pretty_print(os << " -> ", ret_type, indent);
    return os;
}

} // namespace dep0::ast
