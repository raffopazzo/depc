#include "dep0/typecheck/substitute.hpp"
#include "dep0/typecheck/rename.hpp"

#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::typecheck {

static bool contains_var(type_t const&, type_t::var_t const&);

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
            substitute(arr.args.begin(), arr.args.end(), arr.ret_type.get(), var, y);
        });
}

void substitute(
    type_t::arr_t::arg_iterator it,
    type_t::arr_t::arg_iterator const end,
    type_t& ret_type,
    type_t::var_t const& var,
    type_t const& y)
{
    for (; it != end; ++it)
    {
        bool const stop = match(
            it->value,
            [&] (func_arg_t::type_arg_t& type_arg)
            {
                if (type_arg.var == var)
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
                if (type_arg.var and contains_var(y, *type_arg.var))
                    type_arg.var = rename(*type_arg.var, std::next(it), end, ret_type);
                return false;
            },
            [&] (func_arg_t::term_arg_t& term_arg)
            {
                substitute(term_arg.type, var, y);
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
                    arr.args,
                    [&] (func_arg_t const& arg)
                    {
                        return match(
                            arg.value,
                            [&] (func_arg_t::type_arg_t const& x) { return x.var == var; },
                            [&] (func_arg_t::term_arg_t const& x) { return contains_var(x.type, var); });
                    });
        });
}

} // namespace dep0::typecheck
