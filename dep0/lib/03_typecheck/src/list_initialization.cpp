/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/typecheck/list_initialization.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

template <const_t Const>
is_list_initializable_result_t<Const> is_list_initializable_impl(env_t const& env, maybe_const_ref<Const, expr_t> type)
{
    using result_t = is_list_initializable_result_t<Const>;
    auto constexpr no = is_list_initializable_result::no_t{};
    return match(
        type.value,
        [] (expr_t::unit_t) -> result_t
        {
            return is_list_initializable_result::unit_t{};
        },
        [&] (maybe_const_ref<Const, expr_t::app_t> app) -> result_t
        {
            return match(
                app.func.get().value,
                [&] (expr_t::true_t) -> result_t
                {
                    if (auto const c = std::get_if<expr_t::boolean_constant_t>(&app.args[0].value))
                        if (c->value)
                            return is_list_initializable_result::true_t{};
                    return no;
                },
                [&] (expr_t::array_t) -> result_t
                {
                    if (auto const n = std::get_if<expr_t::numeric_constant_t>(&app.args[1].value))
                        return is_list_initializable_result::array_t<Const>{app.args[0], *n};
                    return no;
                },
                [&] (auto const&) -> result_t { return no; });
        },
        [&] (expr_t::global_t const& global) -> result_t
        {
            if (auto const type_def = std::get_if<type_def_t>(env[global]))
                if (auto const s = std::get_if<type_def_t::struct_t>(&type_def->value))
                    return is_list_initializable_result::struct_t{s->fields};
            return no;
        },
        [&] (maybe_const_ref<Const, expr_t::sigma_t> sigma) -> result_t
        {
            return is_list_initializable_result::sigma_t<Const>{sigma.args};
        },
        [&] (maybe_const_ref<Const, expr_t::because_t> because) -> result_t
        {
            return is_list_initializable(env, because.value.get());
        },
        [&] (auto const&) -> result_t { return no; });
}

is_list_initializable_result_t<const_t::yes> is_list_initializable(env_t const& env, expr_t const& type)
{
    return is_list_initializable_impl<const_t::yes>(env, type);
}

is_list_initializable_result_t<const_t::no> is_list_initializable(env_t const& env, expr_t& type)
{
    return is_list_initializable_impl<const_t::no>(env, type);
}

} // namespace dep0::typecheck
