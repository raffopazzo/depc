#include "private/tactics/search_trivial_value.hpp"

#include "private/derivation_rules.hpp"

#include "dep0/typecheck/list_initialization.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

std::optional<expr_t>
search_trivial_value(
    env_t const& env,
    ctx_t const& ctx,
    expr_t const& type,
    search_state_t&,
    usage_t&,
    ast::qty_t)
{
    std::optional<expr_t> result;
    match(
        is_list_initializable(type),
        [&] (is_list_initializable_result::no_t)
        {
            if (auto const global = std::get_if<expr_t::global_t>(&type.value))
                if (auto const type_def = std::get_if<type_def_t>(env[*global]))
                    if (auto const integer = std::get_if<type_def_t::integer_t>(&type_def->value))
                        if (integer->max_abs_value and integer->max_abs_value->is_zero())
                            result = make_legal_expr(type, expr_t::numeric_constant_t{*integer->max_abs_value});
        },
        [&] (is_list_initializable_result::unit_t)
        {
            result = make_legal_expr(type, expr_t::init_list_t{});
        },
        [&] (is_list_initializable_result::true_t)
        {
            result = make_legal_expr(type, expr_t::init_list_t{});
        },
        [&] (is_list_initializable_result::array_t const& array)
        {
            if (array.size.value.is_zero())
                result = make_legal_expr(type, expr_t::init_list_t{});
        });
    return result;
}

} // namespace dep0::typecheck
