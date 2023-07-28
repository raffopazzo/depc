#include "dep0/typecheck/check.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

std::ostream& pretty_print(std::ostream& os, type_t const& t)
{
    match(
        t.value,
        [&] (type_t::bool_t const&) { os << "bool"; },
        [&] (type_t::unit_t const&) { os << "unit_t"; },
        [&] (type_t::i8_t const&) { os << "i8_t"; },
        [&] (type_t::i16_t const&) { os << "i16_t"; },
        [&] (type_t::i32_t const&) { os << "i32_t"; },
        [&] (type_t::i64_t const&) { os << "i64_t"; },
        [&] (type_t::u8_t const&) { os << "u8_t"; },
        [&] (type_t::u16_t const&) { os << "u16_t"; },
        [&] (type_t::u32_t const&) { os << "u32_t"; },
        [&] (type_t::u64_t const&) { os << "u64_t"; },
        [&] (type_t::name_t const& x) { os << x.name; });
    return os;
}

std::ostream& pretty_print(std::ostream& os, expr_t const& x)
{
    match(
        x.value,
        [&] (func_call_t const& x)
        {
            os << x.name << '(';
            bool first = true;
            for (auto const& arg: x.args)
                pretty_print(std::exchange(first, false) ? os : os << ", ", arg);
            os << ')';
        },
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
        [&] (expr_t::numeric_constant_t const& x) { os << x.number; },
        [&] (expr_t::var_t const& x) { os << x.name; });
    return pretty_print(os << " : ", x.properties.type);
}

} // names pace dep0::typecheck
