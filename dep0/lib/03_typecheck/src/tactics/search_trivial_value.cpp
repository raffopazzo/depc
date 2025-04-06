/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/tactics/search_trivial_value.hpp"

#include "private/derivation_rules.hpp"

#include "dep0/typecheck/list_initialization.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

void search_trivial_value(search_task_t& task)
{
    auto const& target = *task.target;
    match(
        is_list_initializable(target),
        [&] (is_list_initializable_result::no_t)
        {
        },
        [&] (is_list_initializable_result::unit_t)
        {
            task.set_result(make_legal_expr(target, expr_t::init_list_t{}));
        },
        [&] (is_list_initializable_result::true_t)
        {
            task.set_result(make_legal_expr(target, expr_t::init_list_t{}));
        },
        [&] (is_list_initializable_result::sigma_const_t const sigma)
        {
            // TODO add a test
            // TODO a tuple whose all elements are trivial values is itself trivial, eg `((); ())` is `{{}, {}}`
            if (sigma.args.empty())
                task.set_result(make_legal_expr(target, expr_t::init_list_t{}));
        },
        [&] (is_list_initializable_result::array_const_t const array)
        {
            if (array.size.value.is_zero())
                task.set_result(make_legal_expr(target, expr_t::init_list_t{}));
        });
}

} // namespace dep0::typecheck
