/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/complete_type.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

expected<std::true_type> is_complete_type(env_t const& env, expr_t const& type)
{
    expected<std::true_type> result;
    match(
        type.value,
        [] (expr_t::typename_t) { },
        [] (expr_t::true_t) { },
        [] (expr_t::auto_t) { },
        [] (expr_t::bool_t) { },
        [] (expr_t::cstr_t) { },
        [] (expr_t::unit_t) { },
        [] (expr_t::i8_t) { },
        [] (expr_t::i16_t) { },
        [] (expr_t::i32_t) { },
        [] (expr_t::i64_t) { },
        [] (expr_t::u8_t) { },
        [] (expr_t::u16_t) { },
        [] (expr_t::u32_t) { },
        [] (expr_t::u64_t) { },
        [] (expr_t::boolean_constant_t const&) { },
        [] (expr_t::numeric_constant_t const&) { },
        [] (expr_t::string_literal_t const&) { },
        [] (expr_t::boolean_expr_t const&) { },
        [] (expr_t::relation_expr_t const&) { },
        [] (expr_t::arith_expr_t const&) { },
        [] (expr_t::var_t const&) { },
        [&] (expr_t::global_t const& g)
        {
            if (auto const p = std::get_if<env_t::incomplete_type_t>(env[g]))
            {
                std::ostringstream err;
                pretty_print(err << '`', type) << "` is incomplete";
                result = error_t(err.str());
            }
        },
        [] (expr_t::app_t const&) { },
        [] (expr_t::abs_t const&) { },
        [] (expr_t::pi_t const&) { },
        [&] (expr_t::sigma_t const& sigma)
        {
            for (auto const& x: sigma.args)
                if (not (result = is_complete_type(env, x.type)))
                    break;
        },
        [] (expr_t::ref_t) { },
        [] (expr_t::scope_t) { },
        [] (expr_t::addressof_t) { },
        [] (expr_t::deref_t) { },
        [] (expr_t::scopeof_t) { },
        [] (expr_t::array_t) { },
        [] (expr_t::init_list_t const&) { },
        [] (expr_t::member_t const&) { },
        [] (expr_t::subscript_t const&) { },
        [&] (expr_t::because_t const& x) { result = is_complete_type(env, x.value.get()); });
    return result;
}

} // namespace dep0::typecheck
