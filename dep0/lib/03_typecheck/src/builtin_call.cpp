#include "dep0/typecheck/builtin_call.hpp"

namespace dep0::typecheck {

is_builtin_call_result_t is_builtin_call(expr_t::app_t const& app)
{
    if (auto const g = std::get_if<typecheck::expr_t::global_t>(&app.func.get().value))
        if (g->module_name and g->module_name->empty() and g->name == "slice")
            return is_builtin_call_result::slice_t{app.args[0], app.args[1], app.args[2], app.args[3], app.args[4]};
    return is_builtin_call_result::no_t{};
}

} // namespace dep0::typecheck
