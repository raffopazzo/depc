#include "private/c_types.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

dep0::expected<std::true_type> is_c_type(parser::expr_t const& x)
{
    auto const no = [&] () -> dep0::expected<std::true_type>
    {
        return dep0::error_t("not a valid C-type", x.properties);
    };
    auto const yes = dep0::expected<std::true_type>{};
    return match(
        x.value,
        [&] (parser::expr_t::typename_t) { return no(); },
        [&] (parser::expr_t::true_t) { return no(); },
        [&] (parser::expr_t::auto_t) { return no(); },
        [&] (parser::expr_t::bool_t) { return yes; },
        [&] (parser::expr_t::cstr_t) { return yes; },
        [&] (parser::expr_t::unit_t) { return no(); },
        [&] (parser::expr_t::i8_t) { return yes; },
        [&] (parser::expr_t::i16_t) { return yes; },
        [&] (parser::expr_t::i32_t) { return yes; },
        [&] (parser::expr_t::i64_t) { return yes; },
        [&] (parser::expr_t::u8_t) { return yes; },
        [&] (parser::expr_t::u16_t) { return yes; },
        [&] (parser::expr_t::u32_t) { return yes; },
        [&] (parser::expr_t::u64_t) { return yes; },
        [&] (parser::expr_t::boolean_constant_t) { return no(); },
        [&] (parser::expr_t::numeric_constant_t) { return no(); },
        [&] (parser::expr_t::string_literal_t) { return no(); },
        [&] (parser::expr_t::boolean_expr_t const&) { return no(); },
        [&] (parser::expr_t::relation_expr_t const&) { return no(); },
        [&] (parser::expr_t::arith_expr_t const&) { return no(); },
        [&] (parser::expr_t::var_t const&) { return no(); },
        [&] (parser::expr_t::global_t const&) { return no(); },
        [&] (parser::expr_t::app_t const&)
        {
            auto const app = get_if_app_of_array(x);
            return app and is_c_type(app->args[0]) ? yes : no();
        },
        [&] (parser::expr_t::abs_t const&) { return no(); },
        [&] (parser::expr_t::pi_t const& pi) { return is_c_func_type(pi); },
        [&] (parser::expr_t::array_t const&) { return no(); },
        [&] (parser::expr_t::init_list_t const&) { return no(); },
        [&] (parser::expr_t::subscript_t const&) { return no(); });
}

dep0::expected<std::true_type> is_c_func_type(parser::expr_t::pi_t const& pi)
{
    for (auto const& arg: pi.args)
        // TODO allow 0 quantity? eg `f(0 typename t, 0 u64_t n, array_t(t, n) p) -> i32_t` could make `p` a `void*`?
        if (auto result = is_c_type(arg.type); not result)
            return result;
    // TODO what about C-functions that return void? shall we allow unit_t for the return type?
    return is_c_type(pi.ret_type.get());
}

} // namespace dep0::typecheck

