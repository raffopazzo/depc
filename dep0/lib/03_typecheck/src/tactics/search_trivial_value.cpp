/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/tactics/search_trivial_value.hpp"

#include "private/derivation_rules.hpp"

#include "dep0/typecheck/list_initialization.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

static std::optional<expr_t> try_make_trivial_value(env_t const& env, expr_t const& type)
{
    return match(
        is_list_initializable(env, type),
        [&] (is_list_initializable_result::no_t) -> std::optional<expr_t>
        {
            return std::nullopt;
        },
        [&] (is_list_initializable_result::unit_t) -> std::optional<expr_t>
        {
            return make_legal_expr(type, expr_t::init_list_t{});
        },
        [&] (is_list_initializable_result::true_t) -> std::optional<expr_t>
        {
            return make_legal_expr(type, expr_t::init_list_t{});
        },
        [&] (is_list_initializable_result::struct_t const& s) -> std::optional<expr_t>
        {
            std::vector<expr_t> values;
            for (auto const& f: s.fields)
                if (auto val = try_make_trivial_value(env, f.type))
                    values.push_back(std::move(*val));
                else
                    return std::nullopt;
            return make_legal_expr(type, expr_t::init_list_t{std::move(values)});
        },
        [&] (is_list_initializable_result::sigma_const_t const sigma) -> std::optional<expr_t>
        {
            std::vector<expr_t> values;
            for (auto const& arg: sigma.args)
                if (auto val = try_make_trivial_value(env, arg.type))
                    values.push_back(std::move(*val));
                else
                    return std::nullopt;
            return make_legal_expr(type, expr_t::init_list_t{std::move(values)});
        },
        [&] (is_list_initializable_result::array_const_t const array) -> std::optional<expr_t>
        {
            return make_legal_expr(type, expr_t::init_list_t{});
        });
}

void search_trivial_value(search_task_t& task)
{
    if (auto val = try_make_trivial_value(task.env, *task.target))
        task.set_result(std::move(*val));
}

} // namespace dep0::typecheck
