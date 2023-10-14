#include "dep0/typecheck/substitute.hpp"

#include "dep0/match.hpp"

#include <algorithm>
#include <cassert>

namespace dep0::typecheck {

static bool contains_var(type_t const&, type_t::var_t const&);
static bool contains_var(type_t::arr_t const&, type_t::var_t const&);
static void rename(type_t::var_t const&, type_t::arr_t&);
static void replace(type_t::var_t const&, type_t::var_t const&, type_t&);
static void replace(type_t::var_t const&, type_t::var_t const&, type_t::arr_t&);
static void substitute(type_t&, type_t::var_t const&, type_t const&);
static void substitute(type_t::arr_t&, type_t::var_t const&, type_t const&);

void substitute(type_t& type, substitution_context_t const& ctx)
{
    assert(ctx.parent().has_value() == false && "Stratified substitution context is not supported");
    for (auto const& [v, t]: ctx)
        substitute(type, v, t);
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
        [&] (type_t::var_t const& v) { return v == var; },
        [&] (type_t::arr_t const& arr) { return contains_var(arr, var); });
}

bool contains_var(type_t::arr_t const& arr, type_t::var_t const& var)
{
    return contains_var(arr.ret_type.get(), var) or
        std::ranges::any_of(
            arr.arg_types,
            [&] (auto const& arg_type)
            {
                return match(
                    arg_type,
                    [&] (typecheck::type_t::var_t const& v) { return v == var; },
                    [&] (typecheck::type_t const& t) { return contains_var(t, var); });
            });
}

void rename(type_t::var_t const& var, type_t::arr_t& arr)
{
    auto new_var = var;
    do
        ++new_var.name.idx;
    while (contains_var(arr, new_var));
    replace(var, new_var, arr);
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
            replace(from, to, arr);
        });
}

void replace(type_t::var_t const& from, type_t::var_t const& to, type_t::arr_t& arr)
{
    for (auto& arg_type: arr.arg_types)
        match(
            arg_type,
            [&] (typecheck::type_t::var_t& v)
            {
                // Note: in theory it would suffice to replace only the free occurrences of `from`
                // and we could stop if `from` introduces a new binding variable;
                // but replacing everything is easier and doesn't harm.
                if (v == from)
                    v = to;
            },
            [&] (typecheck::type_t& t)
            {
                replace(from, to, t);
            });
    return replace(from, to, arr.ret_type.get());
}

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
            substitute(arr, var, y);
        });
}

void substitute(type_t::arr_t& arr, type_t::var_t const& var, type_t const& y)
{
    for (auto& arg_type: arr.arg_types)
    {
        bool const stop =
            match(
                arg_type,
                [&] (typecheck::type_t::var_t const& v)
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
                    while (contains_var(y, v))
                        rename(v, arr); // alternatively could find max index between y and arr and add 1 to it
                    return false;
                },
                [&] (typecheck::type_t& arg_type)
                {
                    substitute(arg_type, var, y);
                    return false;
                });
        if (stop)
            return;
    }
    substitute(arr.ret_type.get(), var, y);
}

} // namespace dep0::typecheck
