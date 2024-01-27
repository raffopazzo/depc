#include "private/first_order_types.hpp"

#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::llvmgen {

bool is_first_order_function_type(std::vector<typecheck::func_arg_t> const& args, typecheck::expr_t const& ret_type)
{
    return
        std::ranges::all_of(args, [] (typecheck::func_arg_t const& arg) { return is_first_order_type(arg.type); })
        and is_first_order_type(ret_type);
}

bool is_first_order_type(typecheck::expr_t const& type)
{
    return match(
        type.value,
        [] (typecheck::expr_t::typename_t const&) { return false; },
        [] (typecheck::expr_t::bool_t const&) { return true; },
        [] (typecheck::expr_t::unit_t const&) { return true; },
        [] (typecheck::expr_t::i8_t const&) { return true; },
        [] (typecheck::expr_t::i16_t const&) { return true; },
        [] (typecheck::expr_t::i32_t const&) { return true; },
        [] (typecheck::expr_t::i64_t const&) { return true; },
        [] (typecheck::expr_t::u8_t const&) { return true; },
        [] (typecheck::expr_t::u16_t const&) { return true; },
        [] (typecheck::expr_t::u32_t const&) { return true; },
        [] (typecheck::expr_t::u64_t const&) { return true; },
        [] (typecheck::expr_t::boolean_constant_t const&) { return false; },
        [] (typecheck::expr_t::numeric_constant_t const&) { return false; },
        [] (typecheck::expr_t::arith_expr_t const&) { return false; },
        [] (typecheck::expr_t::var_t const&) { return true; }, // caller must call only if expr is a type
        [] (typecheck::expr_t::app_t const& x)
        {
            // `array_t(type, size)` is a 1st order type if `type` is of 1st order,
            // regardless of how complex the size expression is;
            // whatever the size, it will always reduce to just a pointer anyway;
            return std::holds_alternative<typecheck::expr_t::array_t>(x.func.get().value)
                and is_first_order_type(x.args[0ul]);
        },
        [] (typecheck::expr_t::abs_t const&) { return false; },
        [] (typecheck::expr_t::pi_t const& t) { return is_first_order_function_type(t.args, t.ret_type.get()); },
        [] (typecheck::expr_t::array_t const&) { return false; }, // `array_t` on its own is a term, not a type
        [] (typecheck::expr_t::init_list_t const&) { return false; },
        [] (typecheck::expr_t::subscript_t const&) { return false; });
}

} // namespace dep0::llvmgen
