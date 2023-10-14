#include "dep0/typecheck/check.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

// TODO indentation
static std::ostream& pretty_print(std::ostream&, body_t const&);
static std::ostream& pretty_print(std::ostream&, stmt_t const&);
static std::ostream& pretty_print(std::ostream&, expr_t::app_t const&);
static std::ostream& pretty_print(std::ostream&, ast::typename_t);

std::ostream& pretty_print(std::ostream& os, body_t const& x)
{
    os << '{';
    for (auto const& s: x.stmts)
        pretty_print(os << std::endl, s);
    os << std::endl << '}';
    return os;
}

std::ostream& pretty_print(std::ostream& os, stmt_t const& x)
{
    match(
        x.value,
        [&] (expr_t::app_t const& x)
        {
            pretty_print(os, x) << ';';
        },
        [&] (stmt_t::if_else_t const& x)
        {
            pretty_print(os << "if (", x.cond) << ')';
            pretty_print(os << std::endl, x.true_branch);
            if (x.false_branch)
                pretty_print(os << "else" << std::endl, *x.false_branch);
        },
        [&] (stmt_t::return_t const& x)
        {
            if (x.expr)
                pretty_print(os << "return ", *x.expr) << ';';
            else
                os << "return;";
        });
    return os;
}

std::ostream& pretty_print(std::ostream& os, expr_t::app_t const& x)
{
    pretty_print(os, x.func.get()) << '(';
    bool first = true;
    for (auto const& arg: x.args)
        pretty_print(std::exchange(first, false) ? os : os << ", ", arg);
    os << ')';
    return os;
}

std::ostream& pretty_print(std::ostream& os, ast::typename_t)
{
    return os << "typename";
}

std::ostream& pretty_print(std::ostream& os, type_def_t const& t)
{
    match(
        t.value,
        [&] (type_def_t::integer_t const& x)
        {
            os << (x.sign == ast::sign_t::signed_v ? "signed" : "unsigned") << ' ';
            os << (
                x.width == ast::width_t::_8 ? "8" :
                x.width == ast::width_t::_16 ? "16" :
                x.width == ast::width_t::_32 ? "32" :
                "64") << " bit integer from ";
            if (x.sign == ast::sign_t::signed_v)
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

std::ostream& pretty_print(std::ostream& os, type_t const& t)
{
    match(t.value, [&] (auto const& x) { pretty_print(os, x); });
    return os;
}

std::ostream& pretty_print(std::ostream& os, type_t::bool_t const&) { return os << "bool"; }
std::ostream& pretty_print(std::ostream& os, type_t::unit_t const&) { return os << "unit_t"; }
std::ostream& pretty_print(std::ostream& os, type_t::i8_t const&) { return os << "i8_t"; }
std::ostream& pretty_print(std::ostream& os, type_t::i16_t const&) { return os << "i16_t"; }
std::ostream& pretty_print(std::ostream& os, type_t::i32_t const&) { return os << "i32_t"; }
std::ostream& pretty_print(std::ostream& os, type_t::i64_t const&) { return os << "i64_t"; }
std::ostream& pretty_print(std::ostream& os, type_t::u8_t const&) { return os << "u8_t"; }
std::ostream& pretty_print(std::ostream& os, type_t::u16_t const&) { return os << "u16_t"; }
std::ostream& pretty_print(std::ostream& os, type_t::u32_t const&) { return os << "u32_t"; }
std::ostream& pretty_print(std::ostream& os, type_t::u64_t const&) { return os << "u64_t"; }
std::ostream& pretty_print(std::ostream& os, type_t::var_t const& x) { return pretty_print(os, x.name); }
std::ostream& pretty_print(std::ostream& os, type_t::arr_t const& x)
{
    os << '(';
    bool first = true;
    for (auto const& t: x.arg_types)
    {
        if (not std::exchange(first, false))
            os << ", ";
        match(t,
            [&] (type_t::var_t const& x) { pretty_print(os << "typename ", x); },
            [&] (type_t const& x) { pretty_print(os, x); });
    }
    return pretty_print(os << ") -> ", x.ret_type.get());
}

std::ostream& pretty_print(std::ostream& os, expr_t const& x)
{
    match(
        x.value,
        [&] (expr_t::arith_expr_t const& x)
        {
            match(
                x.value,
                [&] (expr_t::arith_expr_t::plus_t const& x)
                {
                    pretty_print(os, x.lhs.get());
                    os << " + ";
                    pretty_print(os, x.rhs.get());
                });
        },
        [&] (expr_t::boolean_constant_t const& x) { os << x.value; },
        [&] (expr_t::numeric_constant_t const& x) { (x.sign ? os << *x.sign : os) << x.number; },
        [&] (expr_t::var_t const& x) { pretty_print(os, x.name); },
        [&] (expr_t::app_t const& x)
        {
            pretty_print(os, x);
        },
        [&] (expr_t::abs_t const& x)
        {
            os << '(';
            bool first = true;
            for (auto const& arg: x.args)
            {
                pretty_print(std::exchange(first, false) ? os : os << ", ", arg.sort);
                pretty_print(os << ' ', arg.name);
            }
            pretty_print(os << ") -> ", x.ret_type);
            pretty_print(os << std::endl, x.body);
        },
        [&] (type_t const& x) { pretty_print(os, x); });
    return pretty_print(os << " : ", x.properties.sort);
}

std::ostream& pretty_print(std::ostream& os, sort_t const& x)
{
    match(x, [&] (auto const& x) { pretty_print(os, x); });
    return os;
}

} // namespace dep0::typecheck
