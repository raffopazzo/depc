#pragma once

#include "dep0/ast/pretty_print.hpp"

#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::ast {

namespace detail {

template <Properties P> bool needs_new_line(body_t<P> const&);
template <Properties P> bool needs_new_line(func_arg_t<P> const&);
template <Properties P> bool needs_new_line(expr_t<P> const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::typename_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::bool_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::unit_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::i8_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::i16_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::i32_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::i64_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::u8_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::u16_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::u32_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::u64_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::arith_expr_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::boolean_constant_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::numeric_constant_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::var_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::app_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::abs_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::pi_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::array_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::init_list_t const&);
template <Properties P> bool needs_new_line(typename expr_t<P>::subscript_t const&);

inline std::ostream& new_line(std::ostream& os, std::size_t indent)
{
    os << std::endl;
    while (indent--)
        os << "    ";
    return os;
}

} // namespace detail

template <Properties P>
std::ostream& pretty_print(std::ostream& os, module_t<P> const& module, std::size_t const indent)
{
    bool first = true;
    for (auto const& type_def: module.type_defs)
        pretty_print(std::exchange(first, false) ? os : detail::new_line(os, indent), type_def, indent);
    for (auto const& func_def: module.func_defs)
        pretty_print(std::exchange(first, false) ? os : detail::new_line(os, indent), func_def, indent);
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
    detail::new_line(os, indent);
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
            pretty_print(detail::new_line(os, indent + 1ul), body, indent + 1ul);
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
    pretty_print(os, x.args.begin(), x.args.end(), x.ret_type.get(), indent);
    detail::new_line(os, indent);
    pretty_print(os, x.body, indent);
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, typename expr_t<P>::pi_t const& x, std::size_t const indent)
{
    return pretty_print(os, x.args.begin(), x.args.end(), x.ret_type.get(), indent);
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
std::ostream& pretty_print(
    std::ostream& os,
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
        detail::new_line(os, indent) << ") -> ";
    else
    {
        os << ')';
        (detail::needs_new_line(ret_type) ? detail::new_line(os, indent + 1ul) : os) << " -> ";
    }
    pretty_print(os, ret_type, indent + 1ul); // +1 to help distinguish return type and body
    return os;
}

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

template <Properties P> bool needs_new_line(typename expr_t<P>::typename_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::bool_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::unit_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::i8_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::i16_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::i32_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::i64_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::u8_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::u16_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::u32_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::u64_t const&) { return false; }

template <Properties P>
bool needs_new_line(typename expr_t<P>::arith_expr_t const& x)
{
    return match(
        x.value,
        [&] (typename expr_t<P>::arith_expr_t::plus_t const& x)
        {
            return needs_new_line(x.lhs.get()) or needs_new_line(x.rhs.get());
        });
}

template <Properties P> bool needs_new_line(typename expr_t<P>::boolean_constant_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::numeric_constant_t const&) { return false; }
template <Properties P> bool needs_new_line(typename expr_t<P>::var_t const&) { return false; }

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
bool needs_new_line(typename expr_t<P>::array_t const&)
{
    return false;
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

} // namespace detail

} // namespace dep0::ast