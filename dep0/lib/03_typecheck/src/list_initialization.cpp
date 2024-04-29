#include "dep0/typecheck/list_initialization.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

is_list_initializable_result_t is_list_initializable(expr_t const& type)
{
    auto constexpr no = is_list_initializable_result::no_t{};
    return match(
        type.value,
        [] (expr_t::unit_t) -> is_list_initializable_result_t
        {
            return is_list_initializable_result::unit_t{};
        },
        [&] (expr_t::app_t const& app) -> is_list_initializable_result_t
        {
            return match(
                app.func.get().value,
                [&] (expr_t::true_t) -> is_list_initializable_result_t
                {
                    if (auto const c = std::get_if<expr_t::boolean_constant_t>(&app.args[0].value))
                        if (c->value)
                            return is_list_initializable_result::true_t{};
                    return no;
                },
                [&] (expr_t::array_t) -> is_list_initializable_result_t
                {
                    if (auto const n = std::get_if<expr_t::numeric_constant_t>(&app.args[1].value))
                        return is_list_initializable_result::array_t{app.args[0], *n};
                    return no;
                },
                [&] (auto const&) -> is_list_initializable_result_t { return no; });
        },
        [&] (auto const&) -> is_list_initializable_result_t { return no; });
}

} // namespace dep0::typecheck
