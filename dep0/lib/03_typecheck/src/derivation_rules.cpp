#include "private/derivation_rules.hpp"

namespace dep0::typecheck {

expr_t derivation_rules::make_typename() { return make_legal_expr(kind_t{}, expr_t::typename_t{}); }
expr_t derivation_rules::make_true_t()
{
    return make_legal_expr(
        make_legal_expr(
            kind_t{}, // TODO need to add a test to make sure this is correct
            expr_t::pi_t{std::vector{make_legal_func_arg(make_bool())}, make_typename()}),
        expr_t::true_t{});
}
expr_t derivation_rules::make_true_t(expr_t cond)
{
    return make_legal_expr(make_typename(), expr_t::app_t{make_true_t(), {std::move(cond)}});
}
expr_t derivation_rules::make_bool() { return make_legal_expr(make_typename(), expr_t::bool_t{}); }
expr_t derivation_rules::make_unit() { return make_legal_expr(make_typename(), expr_t::unit_t{}); }
expr_t derivation_rules::make_i8() { return make_legal_expr(make_typename(), expr_t::i8_t{}); }
expr_t derivation_rules::make_i16() { return make_legal_expr(make_typename(), expr_t::i16_t{}); }
expr_t derivation_rules::make_i32() { return make_legal_expr(make_typename(), expr_t::i32_t{}); }
expr_t derivation_rules::make_i64() { return make_legal_expr(make_typename(), expr_t::i64_t{}); }
expr_t derivation_rules::make_u8() { return make_legal_expr(make_typename(), expr_t::u8_t{}); }
expr_t derivation_rules::make_u16() { return make_legal_expr(make_typename(), expr_t::u16_t{}); }
expr_t derivation_rules::make_u32() { return make_legal_expr(make_typename(), expr_t::u32_t{}); }
expr_t derivation_rules::make_u64() { return make_legal_expr(make_typename(), expr_t::u64_t{}); }

expr_t derivation_rules::make_true() { return make_legal_expr(make_bool(), expr_t::boolean_constant_t{true}); }
expr_t derivation_rules::make_false() { return make_legal_expr(make_bool(), expr_t::boolean_constant_t{false}); }

expr_t derivation_rules::make_array()
{
    // the elemnt type of the array must be of sort types,
    // i.e. a "function argument" of type `typename`;
    // to have array of types (eg `{int, bool, bool}`),
    // we fisrt need to add cumulativity of types;
    // i.e. we can have an array of ints, eg {1,2,3},
    // but not an array of types, eg {int, bool, bool};
    // with cumulativity we can have both
    return make_legal_expr(
        make_legal_expr(
            kind_t{}, // TODO need to add a test to make sure this is correct
            expr_t::pi_t{
                std::vector{
                    make_legal_func_arg(make_typename()),
                    make_legal_func_arg(make_u64())
                },
                make_typename()}),
        expr_t::array_t{});
}

} // namespace dep0::typecheck
