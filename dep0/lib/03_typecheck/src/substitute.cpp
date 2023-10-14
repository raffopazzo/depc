#include "dep0/typecheck/substitute.hpp"

#include "dep0/match.hpp"

#include <algorithm>
#include <numeric>

namespace dep0::typecheck {

static bool contains_var(type_t const&, type_t::var_t const&);
static std::size_t max_index(type_t const&);
static std::size_t max_index(
    type_t::arr_t::arg_types_const_iterator begin,
    type_t::arr_t::arg_types_const_iterator end,
    type_t const& ret_type);
static type_t::var_t rename(
    type_t::var_t const&,
    type_t::arr_t::arg_types_iterator begin,
    type_t::arr_t::arg_types_iterator end,
    type_t& ret_type);
static void replace(type_t::var_t const&, type_t::var_t const&, type_t&);
static void replace(
    type_t::var_t const&,
    type_t::var_t const&,
    type_t::arr_t::arg_types_iterator begin,
    type_t::arr_t::arg_types_iterator end,
    type_t& ret_type);

void substitute(type_t& x, type_t::var_t const& var, type_t const& y)
{
    match(
        x.value,
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
        [&] (type_t::var_t const& v)
        {
            if (v == var)
                x = y;
        },
        [&] (type_t::arr_t& arr)
        {
            substitute(arr.arg_types.begin(), arr.arg_types.end(), arr.ret_type.get(), var, y);
        });
}

void substitute(
    type_t::arr_t::arg_types_iterator begin,
    type_t::arr_t::arg_types_iterator const end,
    type_t& ret_type,
    type_t::var_t const& var,
    type_t const& y)
{
    for (; begin != end; ++begin)
    {
        bool const stop = match(
            *begin,
            [&] (type_t::var_t& v)
            {
                if (v == var)
                {
                    // `v` is now a new binding type-variable;
                    // any later arguments refer to this `v` not to the initial `var`;
                    // so substitution must stop, including for the return type
                    return true;
                }
                // We need to check if `v` appears free in `y` and, if so, rename it.
                // Technically here we are actually checking whether `v` appears at all in `y`, i.e.
                // not only free but also as binding variable; this is not strictly necessary,
                // but the code is simpler and it also avoids possibly confusing types like this
                // `(typename t) -> (typename t) -> t` which instead become `(typename t:1) -> (typename t) -> t`,
                // making it more obvious which `t` is binding, so for now let's go with this.
                // Also note that we are modifying the elements of the very vector we are iterating on,
                // but we are only modifying the values, no the vector; so iteration is safe.
                if (contains_var(y, v))
                    v = rename(v, std::next(begin), end, ret_type);
                return false;
            },
            [&] (type_t& arg_type)
            {
                substitute(arg_type, var, y);
                return false;
            });
        if (stop)
            return;
    }
    substitute(ret_type, var, y);
}

bool contains_var(type_t const& type, type_t::var_t const& var)
{
    return match(
        type.value,
        [] (type_t::bool_t const&) { return false; },
        [] (type_t::unit_t const&) { return false; },
        [] (type_t::i8_t const&) { return false; },
        [] (type_t::i16_t const&) { return false; },
        [] (type_t::i32_t const&) { return false; },
        [] (type_t::i64_t const&) { return false; },
        [] (type_t::u8_t const&) { return false; },
        [] (type_t::u16_t const&) { return false; },
        [] (type_t::u32_t const&) { return false; },
        [] (type_t::u64_t const&) { return false; },
        [&] (type_t::var_t const& v)
        {
            return v == var;
        },
        [&] (type_t::arr_t const& arr)
        {
            return contains_var(arr.ret_type.get(), var) or
                std::ranges::any_of(
                    arr.arg_types,
                    [&] (auto const& arg_type)
                    {
                        return match(
                            arg_type,
                            [&] (type_t::var_t const& v) { return v == var; },
                            [&] (type_t const& t) { return contains_var(t, var); });
                    });
        });
}

std::size_t max_index(
    type_t::arr_t::arg_types_const_iterator const begin,
    type_t::arr_t::arg_types_const_iterator const end,
    type_t const& ret_type)
{
    return std::accumulate(
        begin, end,
        max_index(ret_type),
        [] (std::size_t const acc, auto const& arg_type)
        {
            return std::max(
                acc,
                match(
                    arg_type,
                    [] (type_t::var_t const& v) { return v.name.idx; },
                    [] (type_t const& t) { return max_index(t); }));
        });
}

std::size_t max_index(type_t const& type)
{
    return match(
        type.value,
        [] (type_t::bool_t const&) { return 0ul; },
        [] (type_t::unit_t const&) { return 0ul; },
        [] (type_t::i8_t const&) { return 0ul; },
        [] (type_t::i16_t const&) { return 0ul; },
        [] (type_t::i32_t const&) { return 0ul; },
        [] (type_t::i64_t const&) { return 0ul; },
        [] (type_t::u8_t const&) { return 0ul; },
        [] (type_t::u16_t const&) { return 0ul; },
        [] (type_t::u32_t const&) { return 0ul; },
        [] (type_t::u64_t const&) { return 0ul; },
        [] (type_t::var_t const& x) { return x.name.idx; },
        [] (type_t::arr_t const& x) { return max_index(x.arg_types.begin(), x.arg_types.end(), x.ret_type.get()); });
}

type_t::var_t rename(
    type_t::var_t const& var,
    type_t::arr_t::arg_types_iterator const begin,
    type_t::arr_t::arg_types_iterator const end,
    type_t& ret_type)
{
    auto const max_idx = std::max(var.name.idx, max_index(begin, end, ret_type));
    auto const new_var = type_t::var_t{ast::indexed_var_t{var.name.txt, max_idx + 1ul}};
    replace(var, new_var, begin, end, ret_type);
    return new_var;
}

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
            replace(from, to, arr.arg_types.begin(), arr.arg_types.end(), arr.ret_type.get());
        });
}

void replace(
    type_t::var_t const& from,
    type_t::var_t const& to,
    type_t::arr_t::arg_types_iterator const begin,
    type_t::arr_t::arg_types_iterator const end,
    type_t& ret_type)
{
    for (auto& arg_type: std::ranges::subrange(begin, end))
        match(
            arg_type,
            [&] (type_t::var_t& v)
            {
                // Note: in theory it would suffice to replace only the free occurrences of `from`
                // and we could stop if `from` introduces a new binding variable;
                // but replacing everything is easier and it shouldn't harm.
                if (v == from)
                    v = to;
            },
            [&] (type_t& t)
            {
                replace(from, to, t);
            });
    replace(from, to, ret_type);
}

} // namespace dep0::typecheck
