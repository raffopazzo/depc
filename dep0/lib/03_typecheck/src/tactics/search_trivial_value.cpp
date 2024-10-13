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
        [&] (is_list_initializable_result::array_t const& array)
        {
            if (array.size.value.is_zero())
                task.set_result(make_legal_expr(target, expr_t::init_list_t{}));
        });
}

} // namespace dep0::typecheck
