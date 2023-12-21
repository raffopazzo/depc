#include "private/derivation_rules.hpp"

namespace dep0::typecheck {

expr_t derivation_rules::make_typename() { return make_legal_expr(kind_t{}, expr_t::typename_t{}); }
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

} // namespace dep0::typecheck
