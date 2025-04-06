/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/typecheck/list_initialization.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

template <bool Const>
is_list_initializable_result_t<Const> is_list_initializable_impl(maybe_const_ref<Const, expr_t> type)
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
        [&] (maybe_const_ref<Const, expr_t::sigma_t> sigma) -> result_t
        {
            return is_list_initializable_result::sigma_t<Const>{sigma.args};
        },
        [&] (maybe_const_ref<Const, expr_t::because_t> because) -> result_t
        {
            return is_list_initializable(because.value.get());
        },
        [&] (auto const&) -> result_t { return no; });
}

is_list_initializable_result_t<true> is_list_initializable(expr_t const& type)
{
    return is_list_initializable_impl<true>(type);
}

is_list_initializable_result_t<false> is_list_initializable(expr_t& type)
{
    return is_list_initializable_impl<false>(type);
}

} // namespace dep0::typecheck
