#include "dep0/typecheck/substitute.hpp"

#include "dep0/match.hpp"

#include <cassert>

namespace dep0::typecheck {

void substitute(typecheck::type_t& x, substitution_context_t const& ctx)
{
    assert(not ctx.parent().has_value() && "stratified substitution context not yet supported");
    for (auto const& [var, type]: ctx)
        substitute(x, var, type);
}

void substitute(typecheck::type_t& x, typecheck::type_t::var_t const& var, typecheck::type_t const& y)
{
    substitute(x, var.name, y);
}

void substitute(typecheck::type_t& x, source_text const& var, typecheck::type_t const& y)
{
    match(
        x.value,
        [] (typecheck::type_t::bool_t const&) { },
        [] (typecheck::type_t::unit_t const&) { },
        [] (typecheck::type_t::i8_t const&) { },
        [] (typecheck::type_t::i16_t const&) { },
        [] (typecheck::type_t::i32_t const&) { },
        [] (typecheck::type_t::i64_t const&) { },
        [] (typecheck::type_t::u8_t const&) { },
        [] (typecheck::type_t::u16_t const&) { },
        [] (typecheck::type_t::u32_t const&) { },
        [] (typecheck::type_t::u64_t const&) { },
        [&] (typecheck::type_t::var_t const& v)
        {
            if (var == v.name)
                x = y;
        },
        [&] (typecheck::type_t::arr_t& arr)
        {
            for (auto& arg_type: arr.arg_types)
            {
                bool const stop =
                    match(
                        arg_type,
                        [&] (typecheck::type_t::var_t const& v)
                        {
                            if (var == v.name)
                            {
                                // `v` is now a new binding type-variable;
                                // any later arguments refer to this `v` not to the initial `var`;
                                // so substitution must stop, including for the return type
                                return true;
                            }
                            // TODO rename `v` if it appears as free variable in `y`
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
        });
}

} // namespace dep0::typecheck
