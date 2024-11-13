#include "private/prelude.hpp"

#include "dep0/typecheck/check.hpp"

#include "dep0/parser/parse.hpp"
#include "dep0/source.hpp"

namespace dep0::typecheck {

namespace {
auto constexpr prelude_text = R"(
// bool_t (first order logic)
func contradiction(bool_t a, 0 true_t(a), 0 true_t(not a)) -> true_t(false) { if (a) impossible; else impossible; }
func double_neg_intro(bool_t a, 0 true_t(a)) -> true_t(not not a) { if (a) return {}; else impossible; }
func double_neg_elim(bool_t a, 0 true_t(not not a)) -> true_t(a) { if (a) return {}; else impossible; }
func conj_intro(bool_t a, bool_t b, 0 true_t(a), 0 true_t(b)) -> true_t(a and b)
{
    if (a)  if (b) return {};   else impossible;
    else    impossible;
}
func conj_elim_a(bool_t a, bool_t b, 0 true_t(a and b)) -> true_t(a)
{
    if (a)  return {};
    else    if (b) impossible;  else impossible;
}
func conj_elim_b(bool_t a, bool_t b, 0 true_t(a and b)) -> true_t(b)
{
    if (b)  return {};
    else    if (a) impossible;  else impossible;
}
func disj_intro_a(bool_t a, bool_t b, 0 true_t(a)) -> true_t(a or b)
{
    if (a)  if (b) return {};   else return {};
    else    impossible;
}
func disj_intro_b(bool_t a, bool_t b, 0 true_t(b)) -> true_t(a or b)
{
    if (b)  if (a) return {};   else return {};
    else    impossible;
}
func de_morgan_intro_and(bool_t a, bool_t b, 0 true_t(not a or not b)) -> true_t(not (a and b))
{
    if (a)  if (b) impossible;  else return {};
    else    if (b) return {};   else return {};
}
func de_morgan_intro_or(bool_t a, bool_t b, 0 true_t(not a and not b)) -> true_t(not (a or b))
{
    if (a)  if (b) impossible;  else impossible;
    else    if (b) impossible;   else return {};
}
func de_morgan_elim_and(bool_t a, bool_t b, 0 true_t(not (a and b))) -> true_t(not a or not b)
{
    if (a)  if (b) impossible;  else return {};
    else    if (b) return {};   else return {};
}
func de_morgan_elim_or(bool_t a, bool_t b, 0 true_t(not (a or b))) -> true_t(not a and not b)
{
    if (a)  if (b) impossible;  else impossible;
    else    if (b) impossible;   else return {};
}
func modus_ponens(bool_t a, bool_t b, 0 true_t(a) p, (0 true_t(a)) -> true_t(b) f) -> true_t(b)
{
    if (a)
        return f(p);
    else
        impossible;
}
func modus_tollens(bool_t a, bool_t b, 0 true_t(not b) q, (0 true_t(a)) -> true_t(b) f) -> true_t(not a)
{
    if (a)
        impossible because contradiction(b, f({}), q);
    else
        return {};
}
func conjunctive_syllogism_a(bool_t a, bool_t b, 0 true_t(not (a and b)), 0 true_t(b)) -> true_t(not a)
{
    if (b)  if (a) impossible;  else return {};
    else    if (a) impossible;  else return {};
}
func conjunctive_syllogism_b(bool_t a, bool_t b, 0 true_t(not (a and b)), 0 true_t(a)) -> true_t(not b)
{
    if (a)  if (b) impossible;  else return {};
    else    if (b) impossible;  else return {};
}
func disjunctive_syllogism_a(bool_t a, bool_t b, 0 true_t(a or b), 0 true_t(not b)) -> true_t(a)
{
    if (a)  return {};
    else    if (b) impossible;  else impossible;
}
func disjunctive_syllogism_b(bool_t a, bool_t b, 0 true_t(a or b), 0 true_t(not a)) -> true_t(b)
{
    if (b)  return {};
    else    if (a) impossible;  else impossible;
}
func dilemma(
    bool_t a, bool_t b, bool_t c,
    (0 true_t(a)) -> true_t(c) f,
    (0 true_t(b)) -> true_t(c) g,
    0 true_t(a or b)
)
-> true_t(c)
{
    if (a)          return f({});
    else    if (b)  return g({});
            else    impossible;
}
func reductio_ad_absurdum(bool_t a, (0 true_t(a)) -> true_t(false) f) -> true_t(not a)
{
    if (a)  return f({});
    else    return {};
}
func material_implication(bool_t a, bool_t b, (0 true_t(a)) -> true_t(b) f) -> true_t(not a or b)
{
    if (a)  if (b)  return {}; else impossible because f({});
    else    if (b)  return {}; else return {};
}

// bool_t (and)
func and_symm_bool(bool_t a, bool_t b, 0 true_t(a and b)) -> true_t(b and a)
{
    if (a)  if (b) return {};   else impossible;
    else    if (b) impossible;  else impossible;
}
func and_trans_bool(bool_t a, bool_t b, bool_t c, 0 true_t(a and b), 0 true_t(b and c)) -> true_t(a and c)
{
    if (a)
        if (b)  if (c) return {};   else impossible;
        else    if (c) impossible;  else impossible;
    else
        if (b)  impossible;
        else    impossible;
}
func and_true_intro(bool_t a, 0 true_t(a)) -> true_t(a and true) { if (a) return {}; else impossible; }
func and_true_elim(bool_t a, true_t(a and true)) -> true_t(a) { if (a) return {}; else impossible; }

// bool_t (or)
func or_symm_bool(bool_t a, bool_t b, 0 true_t(a or b)) -> true_t(b or a)
{
    if (a)  if (b) return {};   else return {};
    else    if (b) return {};  else impossible;
}
func or_false_intro(bool_t a, 0 true_t(a)) -> true_t(a or false) { if (a) return {}; else impossible; }
func or_false_elim(bool_t a, 0 true_t(a or false)) -> true_t(a) { if (a) return {}; else impossible; }

// bool_t (==)
func eq_refl_bool(bool_t a) -> true_t(a == a) { if (a) return {}; else return {}; }
func eq_symm_bool(bool_t a, bool_t b, 0 true_t(a == b)) -> true_t(b == a)
{
    if (a)  if (b) return {};   else impossible;
    else    if (b) impossible;  else return {};
}
func eq_trans_bool(bool_t a, bool_t b, bool_t c, 0 true_t(a == b), 0 true_t(b == c)) -> true_t(a == c)
{
    if (a)
        if (b)  if (c) return {};   else impossible;
        else    if (a) impossible;  else impossible;
    else
        if (b)  if (c) impossible;  else impossible;
        else    if (c) impossible;  else return {};
}
func eq_true_intro(bool_t a, 0 true_t(a)) -> true_t(a == true) { if (a) return {}; else impossible; }
func eq_true_elim(bool_t a, 0 true_t(a == true)) -> true_t(a) { if (a) return {}; else impossible; }
func eq_false_intro(bool_t a, 0 true_t(not a)) -> true_t(a == false) { if (a) impossible; else return {}; }
func eq_false_elim(bool_t a, 0 true_t(a == false)) -> true_t(not a) { if (a) impossible; else return {}; }
func not_eq_intro(bool_t a, bool_t b, 0 true_t(a != b)) -> true_t(not (a == b))
{
    if (a)  if (b) impossible;  else return {};
    else    if (b) return {};   else impossible;
}
func not_eq_elim(bool_t a, bool_t b, 0 true_t(not (a == b))) -> true_t(a != b)
{
    if (a)  if (b) impossible;  else return {};
    else    if (b) return {};   else impossible;
}
func not_eq_true_intro(bool_t a, 0 true_t(not a)) -> true_t(not (a == true)) { if (a) impossible; else return {}; }
func not_eq_true_elim(bool_t a, 0 true_t(not (a == true))) -> true_t(not a) { if (a) impossible; else return {}; }
func not_eq_false_intro(bool_t a, 0 true_t(a)) -> true_t(not (a == false)) { if (a) return {}; else impossible; }
func not_eq_false_elim(bool_t a, 0 true_t(not (a == false))) -> true_t(a) { if (a) return {}; else impossible; }

// bool_t (!=)
func neq_symm_bool(bool_t a, bool_t b, 0 true_t(a != b)) -> true_t(b != a)
{
    if (a)  if (b) impossible;  else return {};
    else    if (b) return {};   else impossible;
}
func neq_true_intro(bool_t a, 0 true_t(not a)) -> true_t(a != true) { if (a) impossible; else return {}; }
func neq_true_elim(bool_t a, 0 true_t(a != true)) -> true_t(not a) { if (a) impossible; else return {}; }
func neq_false_intro(bool_t a, 0 true_t(a)) -> true_t(a != false) { if (a) return {}; else impossible; }
func neq_false_elim(bool_t a, 0 true_t(a != false)) -> true_t(a) { if (a) return {}; else impossible; }
func not_neq_intro(bool_t a, bool_t b, 0 true_t(a == b)) -> true_t(not (a != b))
{
    if (a)  if (b) return {};   else impossible;
    else    if (b) impossible;  else return {};
}
func not_neq_elim(bool_t a, bool_t b, 0 true_t(not (a != b))) -> true_t(a == b)
{
    if (a)  if (b) return {};   else impossible;
    else    if (b) impossible;  else return {};
}
func not_neq_true_intro(bool_t a, 0 true_t(a)) -> true_t(not (a != true)) { if (a) return {}; else impossible; }

// bool_t (<)
func nothing_smaller_than_false(bool_t a, 0 true_t(a < false)) -> true_t(false) { if (a) impossible; else impossible; }
func not_lt_intro(bool_t a, bool_t b, 0 true_t(a >= b)) -> true_t(not (a < b))
{
    if (a)  if (b)  return {};  else return {};
    else    if (b)  impossible; else return {};
}
func not_lt_elim(bool_t a, bool_t b, 0 true_t(not (a < b))) -> true_t(a >= b)
{
    if (a)  if (b) return {};   else return {};
    else    if (b) impossible;  else return {};
}
func a_lt_b_implies_b(bool_t a, bool_t b, 0 true_t(a < b)) -> true_t(b)
{
    if (a)  if (b) impossible;  else impossible;
    else    if (b) return {};   else impossible;
}

// bool_t (<=)
func lte_refl_bool(bool_t a) -> true_t(a <= a) { if (a) return {}; else return {}; }
func lte_trans_bool(bool_t a, bool_t b, bool_t c, 0 true_t(a <= b), 0 true_t(b <= c)) -> true_t(a <= c)
{
    if (a)
        if (b)  if (c) return{};    else impossible;
        else    if (c) impossible;  else impossible;
    else
        if (b)  if (c) return{};    else impossible;
        else    if (c) return{};    else return{};
}
func true_biggest_bool(bool_t a) -> true_t(a <= true) { if (a) return {}; else return {}; }
func false_smallest_bool(bool_t a) -> true_t(false <= a) { if (a) return {}; else return {}; }

// bool_t (>)
func nothing_bigger_than_true(bool_t a, 0 true_t(a > true)) -> true_t(false) { if (a) impossible; else impossible; }
func not_gt_intro(bool_t a, bool_t b, 0 true_t(a <= b)) -> true_t(not (a > b))
{
    if (a)  if (b) return {};   else impossible;
    else    if (b) return {};   else return {};
}
func not_gt_elim(bool_t a, bool_t b, 0 true_t(not (a > b))) -> true_t(a <= b)
{
    if (a)  if (b) return {};   else impossible;
    else    if (b) return {};   else return {};
}
func a_gt_b_implies_a(bool_t a, bool_t b, 0 true_t(a > b)) -> true_t(a)
{
    if (a)  if (b) impossible; else return {};
    else    if (b) impossible; else impossible;
}

// bool_t (>=)
func gte_refl_bool(bool_t a) -> true_t(a >= a) { if (a) return {}; else return {}; }
func gte_trans_bool(bool_t a, bool_t b, bool_t c, 0 true_t(a >= b), 0 true_t(b >= c)) -> true_t(a >= c)
{
    if (a)
        if (b)  if (c) return {};   else return {};
        else    if (c) impossible;  else return {};
    else
        if (b)  if (c) impossible;  else impossible;
        else    if (c) impossible;  else return {};
}

func lt_gt_commute_bool(bool_t a, bool_t b, 0 true_t(a < b)) -> true_t(b > a)
{
    if (a)  if (b) impossible;  else impossible;
    else    if (b) return {};   else impossible;
}
func lte_gte_commute_bool(bool_t a, bool_t b, 0 true_t(a <= b)) -> true_t(b >= a)
{
    if (a)  if (b) return {};   else impossible;
    else    if (b) return {};   else return {};
}
func gt_lt_commute_bool(bool_t a, bool_t b, 0 true_t(a > b)) -> true_t(b < a)
{
    if (a)  if (b) impossible;  else return {};
    else    if (b) impossible;  else impossible;
}
func gte_lte_commute_bool(bool_t a, bool_t b, 0 true_t(a >= b)) -> true_t(b <= a)
{
    if (a)  if (b) return {};   else return {};
    else    if (b) impossible;  else return {};
}

// integers (==)
axiom eq_refl_i8 ( i8_t a) -> true_t(a == a);
axiom eq_refl_i16(i16_t a) -> true_t(a == a);
axiom eq_refl_i32(i32_t a) -> true_t(a == a);
axiom eq_refl_i64(i64_t a) -> true_t(a == a);
axiom eq_refl_u8 ( u8_t a) -> true_t(a == a);
axiom eq_refl_u16(u16_t a) -> true_t(a == a);
axiom eq_refl_u32(u32_t a) -> true_t(a == a);
axiom eq_refl_u64(u64_t a) -> true_t(a == a);

axiom eq_symm_i8 ( i8_t a,  i8_t b, true_t(a == b)) -> true_t(b == a);
axiom eq_symm_i16(i16_t a, i16_t b, true_t(a == b)) -> true_t(b == a);
axiom eq_symm_i32(i32_t a, i32_t b, true_t(a == b)) -> true_t(b == a);
axiom eq_symm_i64(i64_t a, i64_t b, true_t(a == b)) -> true_t(b == a);
axiom eq_symm_u8 ( u8_t a,  u8_t b, true_t(a == b)) -> true_t(b == a);
axiom eq_symm_u16(u16_t a, u16_t b, true_t(a == b)) -> true_t(b == a);
axiom eq_symm_u32(u32_t a, u32_t b, true_t(a == b)) -> true_t(b == a);
axiom eq_symm_u64(u64_t a, u64_t b, true_t(a == b)) -> true_t(b == a);

axiom eq_trans_i8 ( i8_t a,  i8_t b,  i8_t c, true_t(a == b), true_t(b == c)) -> true_t(a == c);
axiom eq_trans_i16(i16_t a, i16_t b, i16_t c, true_t(a == b), true_t(b == c)) -> true_t(a == c);
axiom eq_trans_i32(i32_t a, i32_t b, i32_t c, true_t(a == b), true_t(b == c)) -> true_t(a == c);
axiom eq_trans_i64(i64_t a, i64_t b, i64_t c, true_t(a == b), true_t(b == c)) -> true_t(a == c);
axiom eq_trans_u8 ( u8_t a,  u8_t b,  u8_t c, true_t(a == b), true_t(b == c)) -> true_t(a == c);
axiom eq_trans_u16(u16_t a, u16_t b, u16_t c, true_t(a == b), true_t(b == c)) -> true_t(a == c);
axiom eq_trans_u32(u32_t a, u32_t b, u32_t c, true_t(a == b), true_t(b == c)) -> true_t(a == c);
axiom eq_trans_u64(u64_t a, u64_t b, u64_t c, true_t(a == b), true_t(b == c)) -> true_t(a == c);

axiom eq_intro_i8 ( i8_t a,  i8_t b, true_t(not (a != b))) -> true_t(a == b);
axiom eq_intro_i16(i16_t a, i16_t b, true_t(not (a != b))) -> true_t(a == b);
axiom eq_intro_i32(i32_t a, i32_t b, true_t(not (a != b))) -> true_t(a == b);
axiom eq_intro_i64(i64_t a, i64_t b, true_t(not (a != b))) -> true_t(a == b);
axiom eq_intro_u8 ( u8_t a,  u8_t b, true_t(not (a != b))) -> true_t(a == b);
axiom eq_intro_u16(u16_t a, u16_t b, true_t(not (a != b))) -> true_t(a == b);
axiom eq_intro_u32(u32_t a, u32_t b, true_t(not (a != b))) -> true_t(a == b);
axiom eq_intro_u64(u64_t a, u64_t b, true_t(not (a != b))) -> true_t(a == b);

axiom eq_elim_i8 ( i8_t a,  i8_t b, true_t(a == b)) -> true_t(not (a != b));
axiom eq_elim_i16(i16_t a, i16_t b, true_t(a == b)) -> true_t(not (a != b));
axiom eq_elim_i32(i32_t a, i32_t b, true_t(a == b)) -> true_t(not (a != b));
axiom eq_elim_i64(i64_t a, i64_t b, true_t(a == b)) -> true_t(not (a != b));
axiom eq_elim_u8 ( u8_t a,  u8_t b, true_t(a == b)) -> true_t(not (a != b));
axiom eq_elim_u16(u16_t a, u16_t b, true_t(a == b)) -> true_t(not (a != b));
axiom eq_elim_u32(u32_t a, u32_t b, true_t(a == b)) -> true_t(not (a != b));
axiom eq_elim_u64(u64_t a, u64_t b, true_t(a == b)) -> true_t(not (a != b));

// integers (!=)
axiom neq_symm_i8 ( i8_t a,  i8_t b, true_t(a != b)) -> true_t(b != a);
axiom neq_symm_i16(i16_t a, i16_t b, true_t(a != b)) -> true_t(b != a);
axiom neq_symm_i32(i32_t a, i32_t b, true_t(a != b)) -> true_t(b != a);
axiom neq_symm_i64(i64_t a, i64_t b, true_t(a != b)) -> true_t(b != a);
axiom neq_symm_u8 ( u8_t a,  u8_t b, true_t(a != b)) -> true_t(b != a);
axiom neq_symm_u16(u16_t a, u16_t b, true_t(a != b)) -> true_t(b != a);
axiom neq_symm_u32(u32_t a, u32_t b, true_t(a != b)) -> true_t(b != a);
axiom neq_symm_u64(u64_t a, u64_t b, true_t(a != b)) -> true_t(b != a);

axiom neq_intro_i8 ( i8_t a,  i8_t b, true_t(not (a == b))) -> true_t(a != b);
axiom neq_intro_i16(i16_t a, i16_t b, true_t(not (a == b))) -> true_t(a != b);
axiom neq_intro_i32(i32_t a, i32_t b, true_t(not (a == b))) -> true_t(a != b);
axiom neq_intro_i64(i64_t a, i64_t b, true_t(not (a == b))) -> true_t(a != b);
axiom neq_intro_u8 ( u8_t a,  u8_t b, true_t(not (a == b))) -> true_t(a != b);
axiom neq_intro_u16(u16_t a, u16_t b, true_t(not (a == b))) -> true_t(a != b);
axiom neq_intro_u32(u32_t a, u32_t b, true_t(not (a == b))) -> true_t(a != b);
axiom neq_intro_u64(u64_t a, u64_t b, true_t(not (a == b))) -> true_t(a != b);

axiom neq_elim_i8 ( i8_t a,  i8_t b, true_t(a != b)) -> true_t(not (a == b));
axiom neq_elim_i16(i16_t a, i16_t b, true_t(a != b)) -> true_t(not (a == b));
axiom neq_elim_i32(i32_t a, i32_t b, true_t(a != b)) -> true_t(not (a == b));
axiom neq_elim_i64(i64_t a, i64_t b, true_t(a != b)) -> true_t(not (a == b));
axiom neq_elim_u8 ( u8_t a,  u8_t b, true_t(a != b)) -> true_t(not (a == b));
axiom neq_elim_u16(u16_t a, u16_t b, true_t(a != b)) -> true_t(not (a == b));
axiom neq_elim_u32(u32_t a, u32_t b, true_t(a != b)) -> true_t(not (a == b));
axiom neq_elim_u64(u64_t a, u64_t b, true_t(a != b)) -> true_t(not (a == b));

// integers (<)
axiom lt_trans_i8 ( i8_t a,  i8_t b,  i8_t c, true_t(a < b), true_t(b < c)) -> true_t(a < c);
axiom lt_trans_i16(i16_t a, i16_t b, i16_t c, true_t(a < b), true_t(b < c)) -> true_t(a < c);
axiom lt_trans_i32(i32_t a, i32_t b, i32_t c, true_t(a < b), true_t(b < c)) -> true_t(a < c);
axiom lt_trans_i64(i64_t a, i64_t b, i64_t c, true_t(a < b), true_t(b < c)) -> true_t(a < c);
axiom lt_trans_u8 ( u8_t a,  u8_t b,  u8_t c, true_t(a < b), true_t(b < c)) -> true_t(a < c);
axiom lt_trans_u16(u16_t a, u16_t b, u16_t c, true_t(a < b), true_t(b < c)) -> true_t(a < c);
axiom lt_trans_u32(u32_t a, u32_t b, u32_t c, true_t(a < b), true_t(b < c)) -> true_t(a < c);
axiom lt_trans_u64(u64_t a, u64_t b, u64_t c, true_t(a < b), true_t(b < c)) -> true_t(a < c);

axiom lt_intro_i8 ( i8_t a,  i8_t b, true_t(not (a >= b))) -> true_t(a < b);
axiom lt_intro_i16(i16_t a, i16_t b, true_t(not (a >= b))) -> true_t(a < b);
axiom lt_intro_i32(i32_t a, i32_t b, true_t(not (a >= b))) -> true_t(a < b);
axiom lt_intro_i64(i64_t a, i64_t b, true_t(not (a >= b))) -> true_t(a < b);
axiom lt_intro_u8 ( u8_t a,  u8_t b, true_t(not (a >= b))) -> true_t(a < b);
axiom lt_intro_u16(u16_t a, u16_t b, true_t(not (a >= b))) -> true_t(a < b);
axiom lt_intro_u32(u32_t a, u32_t b, true_t(not (a >= b))) -> true_t(a < b);
axiom lt_intro_u64(u64_t a, u64_t b, true_t(not (a >= b))) -> true_t(a < b);

axiom lt_elim_i8 ( i8_t a,  i8_t b, true_t(a < b)) -> true_t(not (a >= b));
axiom lt_elim_i16(i16_t a, i16_t b, true_t(a < b)) -> true_t(not (a >= b));
axiom lt_elim_i32(i32_t a, i32_t b, true_t(a < b)) -> true_t(not (a >= b));
axiom lt_elim_i64(i64_t a, i64_t b, true_t(a < b)) -> true_t(not (a >= b));
axiom lt_elim_u8 ( u8_t a,  u8_t b, true_t(a < b)) -> true_t(not (a >= b));
axiom lt_elim_u16(u16_t a, u16_t b, true_t(a < b)) -> true_t(not (a >= b));
axiom lt_elim_u32(u32_t a, u32_t b, true_t(a < b)) -> true_t(not (a >= b));
axiom lt_elim_u64(u64_t a, u64_t b, true_t(a < b)) -> true_t(not (a >= b));

axiom lt_gt_commute_i8 ( i8_t a,  i8_t b, true_t(a < b)) -> true_t(b > a);
axiom lt_gt_commute_i16(i16_t a, i16_t b, true_t(a < b)) -> true_t(b > a);
axiom lt_gt_commute_i32(i32_t a, i32_t b, true_t(a < b)) -> true_t(b > a);
axiom lt_gt_commute_i64(i64_t a, i64_t b, true_t(a < b)) -> true_t(b > a);
axiom lt_gt_commute_u8 ( u8_t a,  u8_t b, true_t(a < b)) -> true_t(b > a);
axiom lt_gt_commute_u16(u16_t a, u16_t b, true_t(a < b)) -> true_t(b > a);
axiom lt_gt_commute_u32(u32_t a, u32_t b, true_t(a < b)) -> true_t(b > a);
axiom lt_gt_commute_u64(u64_t a, u64_t b, true_t(a < b)) -> true_t(b > a);

// integers (<=)
axiom lte_trans_i8 ( i8_t a,  i8_t b,  i8_t c, true_t(a <= b), true_t(b <= c)) -> true_t(a <= c);
axiom lte_trans_i16(i16_t a, i16_t b, i16_t c, true_t(a <= b), true_t(b <= c)) -> true_t(a <= c);
axiom lte_trans_i32(i32_t a, i32_t b, i32_t c, true_t(a <= b), true_t(b <= c)) -> true_t(a <= c);
axiom lte_trans_i64(i64_t a, i64_t b, i64_t c, true_t(a <= b), true_t(b <= c)) -> true_t(a <= c);
axiom lte_trans_u8 ( u8_t a,  u8_t b,  u8_t c, true_t(a <= b), true_t(b <= c)) -> true_t(a <= c);
axiom lte_trans_u16(u16_t a, u16_t b, u16_t c, true_t(a <= b), true_t(b <= c)) -> true_t(a <= c);
axiom lte_trans_u32(u32_t a, u32_t b, u32_t c, true_t(a <= b), true_t(b <= c)) -> true_t(a <= c);
axiom lte_trans_u64(u64_t a, u64_t b, u64_t c, true_t(a <= b), true_t(b <= c)) -> true_t(a <= c);

axiom lte_intro_i8 ( i8_t a,  i8_t b, true_t(not (a > b))) -> true_t(a <= b);
axiom lte_intro_i16(i16_t a, i16_t b, true_t(not (a > b))) -> true_t(a <= b);
axiom lte_intro_i32(i32_t a, i32_t b, true_t(not (a > b))) -> true_t(a <= b);
axiom lte_intro_i64(i64_t a, i64_t b, true_t(not (a > b))) -> true_t(a <= b);
axiom lte_intro_u8 ( u8_t a,  u8_t b, true_t(not (a > b))) -> true_t(a <= b);
axiom lte_intro_u16(u16_t a, u16_t b, true_t(not (a > b))) -> true_t(a <= b);
axiom lte_intro_u32(u32_t a, u32_t b, true_t(not (a > b))) -> true_t(a <= b);
axiom lte_intro_u64(u64_t a, u64_t b, true_t(not (a > b))) -> true_t(a <= b);

axiom lte_elim_i8 ( i8_t a,  i8_t b, true_t(a <= b)) -> true_t(not (a > b));
axiom lte_elim_i16(i16_t a, i16_t b, true_t(a <= b)) -> true_t(not (a > b));
axiom lte_elim_i32(i32_t a, i32_t b, true_t(a <= b)) -> true_t(not (a > b));
axiom lte_elim_i64(i64_t a, i64_t b, true_t(a <= b)) -> true_t(not (a > b));
axiom lte_elim_u8 ( u8_t a,  u8_t b, true_t(a <= b)) -> true_t(not (a > b));
axiom lte_elim_u16(u16_t a, u16_t b, true_t(a <= b)) -> true_t(not (a > b));
axiom lte_elim_u32(u32_t a, u32_t b, true_t(a <= b)) -> true_t(not (a > b));
axiom lte_elim_u64(u64_t a, u64_t b, true_t(a <= b)) -> true_t(not (a > b));

axiom lte_gte_commute_i8 ( i8_t a,  i8_t b, true_t(a <= b)) -> true_t(b >= a);
axiom lte_gte_commute_i16(i16_t a, i16_t b, true_t(a <= b)) -> true_t(b >= a);
axiom lte_gte_commute_i32(i32_t a, i32_t b, true_t(a <= b)) -> true_t(b >= a);
axiom lte_gte_commute_i64(i64_t a, i64_t b, true_t(a <= b)) -> true_t(b >= a);
axiom lte_gte_commute_u8 ( u8_t a,  u8_t b, true_t(a <= b)) -> true_t(b >= a);
axiom lte_gte_commute_u16(u16_t a, u16_t b, true_t(a <= b)) -> true_t(b >= a);
axiom lte_gte_commute_u32(u32_t a, u32_t b, true_t(a <= b)) -> true_t(b >= a);
axiom lte_gte_commute_u64(u64_t a, u64_t b, true_t(a <= b)) -> true_t(b >= a);

// integers (>)
axiom gt_trans_i8 ( i8_t a,  i8_t b,  i8_t c, true_t(a > b), true_t(b > c)) -> true_t(a > c);
axiom gt_trans_i16(i16_t a, i16_t b, i16_t c, true_t(a > b), true_t(b > c)) -> true_t(a > c);
axiom gt_trans_i32(i32_t a, i32_t b, i32_t c, true_t(a > b), true_t(b > c)) -> true_t(a > c);
axiom gt_trans_i64(i64_t a, i64_t b, i64_t c, true_t(a > b), true_t(b > c)) -> true_t(a > c);
axiom gt_trans_u8 ( u8_t a,  u8_t b,  u8_t c, true_t(a > b), true_t(b > c)) -> true_t(a > c);
axiom gt_trans_u16(u16_t a, u16_t b, u16_t c, true_t(a > b), true_t(b > c)) -> true_t(a > c);
axiom gt_trans_u32(u32_t a, u32_t b, u32_t c, true_t(a > b), true_t(b > c)) -> true_t(a > c);
axiom gt_trans_u64(u64_t a, u64_t b, u64_t c, true_t(a > b), true_t(b > c)) -> true_t(a > c);

axiom gt_intro_i8 ( i8_t a,  i8_t b, true_t(not (a <= b))) -> true_t(a > b);
axiom gt_intro_i16(i16_t a, i16_t b, true_t(not (a <= b))) -> true_t(a > b);
axiom gt_intro_i32(i32_t a, i32_t b, true_t(not (a <= b))) -> true_t(a > b);
axiom gt_intro_i64(i64_t a, i64_t b, true_t(not (a <= b))) -> true_t(a > b);
axiom gt_intro_u8 ( u8_t a,  u8_t b, true_t(not (a <= b))) -> true_t(a > b);
axiom gt_intro_u16(u16_t a, u16_t b, true_t(not (a <= b))) -> true_t(a > b);
axiom gt_intro_u32(u32_t a, u32_t b, true_t(not (a <= b))) -> true_t(a > b);
axiom gt_intro_u64(u64_t a, u64_t b, true_t(not (a <= b))) -> true_t(a > b);

axiom gt_elim_i8 ( i8_t a,  i8_t b, true_t(a > b)) -> true_t(not (a <= b));
axiom gt_elim_i16(i16_t a, i16_t b, true_t(a > b)) -> true_t(not (a <= b));
axiom gt_elim_i32(i32_t a, i32_t b, true_t(a > b)) -> true_t(not (a <= b));
axiom gt_elim_i64(i64_t a, i64_t b, true_t(a > b)) -> true_t(not (a <= b));
axiom gt_elim_u8 ( u8_t a,  u8_t b, true_t(a > b)) -> true_t(not (a <= b));
axiom gt_elim_u16(u16_t a, u16_t b, true_t(a > b)) -> true_t(not (a <= b));
axiom gt_elim_u32(u32_t a, u32_t b, true_t(a > b)) -> true_t(not (a <= b));
axiom gt_elim_u64(u64_t a, u64_t b, true_t(a > b)) -> true_t(not (a <= b));

axiom gt_lt_commute_i8 ( i8_t a,  i8_t b, true_t(a > b)) -> true_t(b < a);
axiom gt_lt_commute_i16(i16_t a, i16_t b, true_t(a > b)) -> true_t(b < a);
axiom gt_lt_commute_i32(i32_t a, i32_t b, true_t(a > b)) -> true_t(b < a);
axiom gt_lt_commute_i64(i64_t a, i64_t b, true_t(a > b)) -> true_t(b < a);
axiom gt_lt_commute_u8 ( u8_t a,  u8_t b, true_t(a > b)) -> true_t(b < a);
axiom gt_lt_commute_u16(u16_t a, u16_t b, true_t(a > b)) -> true_t(b < a);
axiom gt_lt_commute_u32(u32_t a, u32_t b, true_t(a > b)) -> true_t(b < a);
axiom gt_lt_commute_u64(u64_t a, u64_t b, true_t(a > b)) -> true_t(b < a);

// integers (>=)
axiom gte_trans_i8 ( i8_t a,  i8_t b,  i8_t c, true_t(a >= b), true_t(b >= c)) -> true_t(a >= c);
axiom gte_trans_i16(i16_t a, i16_t b, i16_t c, true_t(a >= b), true_t(b >= c)) -> true_t(a >= c);
axiom gte_trans_i32(i32_t a, i32_t b, i32_t c, true_t(a >= b), true_t(b >= c)) -> true_t(a >= c);
axiom gte_trans_i64(i64_t a, i64_t b, i64_t c, true_t(a >= b), true_t(b >= c)) -> true_t(a >= c);
axiom gte_trans_u8 ( u8_t a,  u8_t b,  u8_t c, true_t(a >= b), true_t(b >= c)) -> true_t(a >= c);
axiom gte_trans_u16(u16_t a, u16_t b, u16_t c, true_t(a >= b), true_t(b >= c)) -> true_t(a >= c);
axiom gte_trans_u32(u32_t a, u32_t b, u32_t c, true_t(a >= b), true_t(b >= c)) -> true_t(a >= c);
axiom gte_trans_u64(u64_t a, u64_t b, u64_t c, true_t(a >= b), true_t(b >= c)) -> true_t(a >= c);

axiom gte_intro_i8 ( i8_t a,  i8_t b, true_t(not (a < b))) -> true_t(a >= b);
axiom gte_intro_i16(i16_t a, i16_t b, true_t(not (a < b))) -> true_t(a >= b);
axiom gte_intro_i32(i32_t a, i32_t b, true_t(not (a < b))) -> true_t(a >= b);
axiom gte_intro_i64(i64_t a, i64_t b, true_t(not (a < b))) -> true_t(a >= b);
axiom gte_intro_u8 ( u8_t a,  u8_t b, true_t(not (a < b))) -> true_t(a >= b);
axiom gte_intro_u16(u16_t a, u16_t b, true_t(not (a < b))) -> true_t(a >= b);
axiom gte_intro_u32(u32_t a, u32_t b, true_t(not (a < b))) -> true_t(a >= b);
axiom gte_intro_u64(u64_t a, u64_t b, true_t(not (a < b))) -> true_t(a >= b);

axiom gte_elim_i8 ( i8_t a,  i8_t b, true_t(a >= b)) -> true_t(not (a < b));
axiom gte_elim_i16(i16_t a, i16_t b, true_t(a >= b)) -> true_t(not (a < b));
axiom gte_elim_i32(i32_t a, i32_t b, true_t(a >= b)) -> true_t(not (a < b));
axiom gte_elim_i64(i64_t a, i64_t b, true_t(a >= b)) -> true_t(not (a < b));
axiom gte_elim_u8 ( u8_t a,  u8_t b, true_t(a >= b)) -> true_t(not (a < b));
axiom gte_elim_u16(u16_t a, u16_t b, true_t(a >= b)) -> true_t(not (a < b));
axiom gte_elim_u32(u32_t a, u32_t b, true_t(a >= b)) -> true_t(not (a < b));
axiom gte_elim_u64(u64_t a, u64_t b, true_t(a >= b)) -> true_t(not (a < b));

axiom gte_lte_commute_i8 ( i8_t a,  i8_t b, true_t(a >= b)) -> true_t(b <= a);
axiom gte_lte_commute_i16(i16_t a, i16_t b, true_t(a >= b)) -> true_t(b <= a);
axiom gte_lte_commute_i32(i32_t a, i32_t b, true_t(a >= b)) -> true_t(b <= a);
axiom gte_lte_commute_i64(i64_t a, i64_t b, true_t(a >= b)) -> true_t(b <= a);
axiom gte_lte_commute_u8 ( u8_t a,  u8_t b, true_t(a >= b)) -> true_t(b <= a);
axiom gte_lte_commute_u16(u16_t a, u16_t b, true_t(a >= b)) -> true_t(b <= a);
axiom gte_lte_commute_u32(u32_t a, u32_t b, true_t(a >= b)) -> true_t(b <= a);
axiom gte_lte_commute_u64(u64_t a, u64_t b, true_t(a >= b)) -> true_t(b <= a);

// plus and minus for integers

axiom plus_1_lte_intro_i8 (i8_t  a, i8_t  b, true_t(a < b)) -> true_t(a + 1 <= b);
axiom plus_1_lte_intro_i16(i16_t a, i16_t b, true_t(a < b)) -> true_t(a + 1 <= b);
axiom plus_1_lte_intro_i32(i32_t a, i32_t b, true_t(a < b)) -> true_t(a + 1 <= b);
axiom plus_1_lte_intro_i64(i64_t a, i64_t b, true_t(a < b)) -> true_t(a + 1 <= b);
axiom plus_1_lte_intro_u8 (u8_t  a, u8_t  b, true_t(a < b)) -> true_t(a + 1 <= b);
axiom plus_1_lte_intro_u16(u16_t a, u16_t b, true_t(a < b)) -> true_t(a + 1 <= b);
axiom plus_1_lte_intro_u32(u32_t a, u32_t b, true_t(a < b)) -> true_t(a + 1 <= b);
axiom plus_1_lte_intro_u64(u64_t a, u64_t b, true_t(a < b)) -> true_t(a + 1 <= b);

axiom plus_1_lte_elim_i8 ( i8_t a,  i8_t b, true_t(a + 1 <= b), true_t(a < 127)) -> true_t(a < b);
axiom plus_1_lte_elim_i16(i16_t a, i16_t b, true_t(a + 1 <= b), true_t(a < 32767)) -> true_t(a < b);
axiom plus_1_lte_elim_i32(i32_t a, i32_t b, true_t(a + 1 <= b), true_t(a < 2147483647)) -> true_t(a < b);
axiom plus_1_lte_elim_i64(i64_t a, i64_t b, true_t(a + 1 <= b), true_t(a < 9223372036854775807)) -> true_t(a < b);
axiom plus_1_lte_elim_u8 ( u8_t a,  u8_t b, true_t(a + 1 <= b), true_t(a < 255)) -> true_t(a < b);
axiom plus_1_lte_elim_u16(u16_t a, u16_t b, true_t(a + 1 <= b), true_t(a < 65535)) -> true_t(a < b);
axiom plus_1_lte_elim_u32(u32_t a, u32_t b, true_t(a + 1 <= b), true_t(a < 4294967295)) -> true_t(a < b);
axiom plus_1_lte_elim_u64(u64_t a, u64_t b, true_t(a + 1 <= b), true_t(a < 18446744073709551615)) -> true_t(a < b);

axiom minus_1_gte_intro_i8 (i8_t  a, i8_t  b, true_t(a > b)) -> true_t(a - 1 >= b);
axiom minus_1_gte_intro_i16(i16_t a, i16_t b, true_t(a > b)) -> true_t(a - 1 >= b);
axiom minus_1_gte_intro_i32(i32_t a, i32_t b, true_t(a > b)) -> true_t(a - 1 >= b);
axiom minus_1_gte_intro_i64(i64_t a, i64_t b, true_t(a > b)) -> true_t(a - 1 >= b);
axiom minus_1_gte_intro_u8 (u8_t  a, u8_t  b, true_t(a > b)) -> true_t(a - 1 >= b);
axiom minus_1_gte_intro_u16(u16_t a, u16_t b, true_t(a > b)) -> true_t(a - 1 >= b);
axiom minus_1_gte_intro_u32(u32_t a, u32_t b, true_t(a > b)) -> true_t(a - 1 >= b);
axiom minus_1_gte_intro_u64(u64_t a, u64_t b, true_t(a > b)) -> true_t(a - 1 >= b);

axiom minus_1_gte_elim_i8 (i8_t  a, i8_t  b, true_t(a - 1 >= b), true_t(a > -128)) -> true_t(a > b);
axiom minus_1_gte_elim_i16(i16_t a, i16_t b, true_t(a - 1 >= b), true_t(a > -32768)) -> true_t(a > b);
axiom minus_1_gte_elim_i32(i32_t a, i32_t b, true_t(a - 1 >= b), true_t(a > -2147483648)) -> true_t(a > b);
axiom minus_1_gte_elim_i64(i64_t a, i64_t b, true_t(a - 1 >= b), true_t(a > -9223372036854775808)) -> true_t(a > b);
axiom minus_1_gte_elim_u8 (u8_t  a, u8_t  b, true_t(a - 1 >= b), true_t(a > 0)) -> true_t(a > b);
axiom minus_1_gte_elim_u16(u16_t a, u16_t b, true_t(a - 1 >= b), true_t(a > 0)) -> true_t(a > b);
axiom minus_1_gte_elim_u32(u32_t a, u32_t b, true_t(a - 1 >= b), true_t(a > 0)) -> true_t(a > b);
axiom minus_1_gte_elim_u64(u64_t a, u64_t b, true_t(a - 1 >= b), true_t(a > 0)) -> true_t(a > b);

// array_t
func [[builtin]] slice(0 typename t, 0 u64_t n, array_t(t, n), u64_t k, 0 true_t(n > k)) -> array_t(t, n - k);

)";
} // namespace

dep0::expected<module_t> build_prelude_module()
{
    auto const parse_result = parser::parse(source_text::from_literal(prelude_text));
    if (not parse_result)
        return dep0::error_t("error parsing prelude module", {std::move(parse_result.error())});
    auto const check_result = check(env_t{}, *parse_result);
    if (not check_result)
        return dep0::error_t("error type-checking prelude module", {std::move(check_result.error())});
    return *check_result;
}

} // namespace dep0::typecheck
