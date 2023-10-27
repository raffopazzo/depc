#include "dep0/typecheck/replace.hpp"

#include "dep0/match.hpp"

#include <ranges>

namespace dep0::typecheck {

void replace(type_t::var_t const& from, type_t::var_t const& to, type_t& type)
{
    match(
        type.value,
        [] (type_t::bool_t const&) { },
        [] (type_t::unit_t const&) { },
        [] (type_t::i8_t const&) { },
        [] (type_t::i16_t const&) { },
        [] (type_t::i32_t const&) { },
        [] (type_t::i64_t const&) { },
        [] (type_t::u8_t const&) { },
        [] (type_t::u16_t const&) { },
        [] (type_t::u32_t const&) { },
        [] (type_t::u64_t const&) { },
        [&] (type_t::var_t& v)
        {
            if (v == from)
                v = to;
        },
        [&] (type_t::arr_t& arr)
        {
            replace(from, to, arr.args.begin(), arr.args.end(), arr.ret_type.get());
        });
}

void replace(
    type_t::var_t const& from,
    type_t::var_t const& to,
    type_t::arr_t::arg_iterator const begin,
    type_t::arr_t::arg_iterator const end,
    type_t& ret_type)
{
    for (auto& arg: std::ranges::subrange(begin, end))
        match(
            arg.sort,
            [&] (ast::typename_t)
            {
                // Note: in theory it would suffice to replace only the free occurrences of `from`
                // and we could stop if `from` introduces a new binding variable;
                // but replacing everything is easier and it shouldn't harm.
                if (arg.name == from.name)
                    arg.name = to.name;
            },
            [&] (type_t& t)
            {
                replace(from, to, t);
            });
    replace(from, to, ret_type);
}

} // namespace dep0::typecheck
