#pragma once

#include "dep0/ast/ast.hpp"

#include "dep0/match.hpp"

#include <ostream>

namespace dep0::ast {

// forward declarations

std::ostream& pretty_print(std::ostream&, indexed_var_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, module_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, type_def_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, func_def_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, sort_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, type_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename type_t<P>::bool_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename type_t<P>::unit_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename type_t<P>::i8_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename type_t<P>::i16_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename type_t<P>::i32_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename type_t<P>::i64_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename type_t<P>::u8_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename type_t<P>::u16_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename type_t<P>::u32_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename type_t<P>::u64_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename type_t<P>::var_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename type_t<P>::arr_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, body_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, stmt_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, expr_t<P> const&, std::size_t indent = 0ul);

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
            os << (x.sign == sign_t::signed_v ? "signed" : "unsigned") << ' ';
            os << (
                x.width == width_t::_8 ? "8" :
                x.width == width_t::_16 ? "16" :
                x.width == width_t::_32 ? "32" :
                "64") << " bit integer from ";
            if (x.sign == sign_t::signed_v)
            {
                if (x.max_abs_value)
                    os << '-' << *x.max_abs_value << " to " << *x.max_abs_value;
                else
                    os << "... to ...";
            }
            else
                os << "0 to " << (x.max_abs_value ? x.max_abs_value->view() : "...");
        });
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, func_def_t<P> const& func_def, std::size_t const indent)
{
    os << "auto " << func_def.name;
    if (func_def.value.args.empty())
        os << "()";
    else
    {
        os << '(';
        bool first = true;
        for (auto const& arg: func_def.value.args)
        {
            new_line(std::exchange(first, false) ? os : os << ',', indent + 1ul);
            pretty_print(os, arg.sort, indent + 1ul);
            pretty_print(os << ' ', arg.name);
        }
        new_line(os, indent) << ')';
    }
    pretty_print(os << " -> ", func_def.value.ret_type, indent);
    new_line(os, indent);
    pretty_print(os, func_def.value.body, indent);
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, sort_t<P> const& x, std::size_t const indent)
{
    match(
        x,
        [&] (typename_t const&) { os << "typename"; },
        [&] (type_t<P> const& t) { pretty_print(os, t, indent); });
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, type_t<P> const& type, std::size_t const indent)
{
    match(type.value, [&] (auto const& x) { pretty_print<P>(os, x, indent); });
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename type_t<P>::bool_t const& x, std::size_t)
{
    return os << "bool";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename type_t<P>::unit_t const& x, std::size_t)
{
    return os << "unit_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename type_t<P>::i8_t const& x, std::size_t)
{
    return os << "i8_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename type_t<P>::i16_t const& x, std::size_t)
{
    return os << "i16_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename type_t<P>::i32_t const& x, std::size_t)
{
    return os << "i32_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename type_t<P>::i64_t const& x, std::size_t)
{
    return os << "i64_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename type_t<P>::u8_t const& x, std::size_t)
{
    return os << "u8_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename type_t<P>::u16_t const& x, std::size_t)
{
    return os << "u16_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename type_t<P>::u32_t const& x, std::size_t)
{
    return os << "u32_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename type_t<P>::u64_t const& x, std::size_t)
{
    return os << "u64_t";
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename type_t<P>::var_t const& x, std::size_t)
{
    return pretty_print(os, x.name);
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename type_t<P>::arr_t const& x, std::size_t const indent)
{
    os << '(';
    bool first = true;
    for (auto const& t: x.arg_types)
    {
        if (not std::exchange(first, false))
            os << ", ";
        match(t,
            [&] (typename type_t<P>::var_t const& x) { pretty_print(os << "typename ", x.name); },
            [&] (type_t<P> const& x) { pretty_print(os, x, indent); });
    }
    pretty_print(os << ") -> ", x.ret_type.get(), indent);
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, body_t<P> const& x, std::size_t const indent)
{
    os << '{';
    for (auto const& s: x.stmts)
        pretty_print(new_line(os, indent + 1ul), s, indent + 1ul);
    new_line(os, indent) << '}';
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, stmt_t<P> const& x, std::size_t const indent)
{
    match(
        x.value,
        [&] (typename expr_t<P>::app_t const& x)
        {
            pretty_print<P>(os, x, indent) << ';';
        },
        [&] (typename stmt_t<P>::if_else_t const& x)
        {
            pretty_print(os << "if (", x.cond, indent + 1ul) << ')';
            pretty_print(new_line(os, indent), x.true_branch);
            if (x.false_branch)
            {
                new_line(os, indent) << "else";
                pretty_print(new_line(os, indent), *x.false_branch);
            }
        },
        [&] (typename stmt_t<P>::return_t const& x)
        {
            if (x.expr)
                pretty_print(os << "return ", *x.expr, indent + 1ul) << ';';
            else
                os << "return;";
        });
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
    return os << x.value;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::numeric_constant_t const& x, std::size_t const indent)
{
    return (x.sign ? os << *x.sign : os) << x.number;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::var_t const& x, std::size_t const indent)
{
    return pretty_print(os, x.name);
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::app_t const& x, std::size_t const indent)
{
    pretty_print(os, x.func.get(), indent) << '(';
    bool first = true;
    for (auto const& arg: x.args)
        pretty_print(std::exchange(first, false) ? os : os << ", ", arg, indent + 1ul);
    os << ')';
    return os;
}


template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::abs_t const& x, std::size_t const indent)
{
    os << "[] ";
    if (x.args.empty())
        os << "()";
    else
    {
        os << '(';
        bool first = true;
        for (auto const& arg: x.args)
        {
            new_line(std::exchange(first, false) ? os : os << ',', indent + 1ul);
            pretty_print(os, arg.sort, indent + 1ul);
            pretty_print(os << ' ', arg.name);
        }
        new_line(os, indent) << ')';
    }
    pretty_print(os << " -> ", x.ret_type, indent);
    new_line(os, indent);
    pretty_print(os, x.body, indent);
    return os;
}

} // namespace dep0::ast
