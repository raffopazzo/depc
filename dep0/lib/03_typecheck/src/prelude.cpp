#include "private/prelude.hpp"

#include "dep0/typecheck/check.hpp"

#include "dep0/parser/parse.hpp"
#include "dep0/source.hpp"

namespace dep0::typecheck {

namespace {
auto constexpr prelude_text = R"(
// bool (first order logic)
auto contradiction(bool a, 0 true_t(a), 0 true_t(not a)) -> true_t(false) { if (a) impossible; else impossible; }
auto double_neg_intro(bool a, 0 true_t(a)) -> true_t(not not a) { if (a) return {}; else impossible; }
auto double_neg_elim(bool a, 0 true_t(not not a)) -> true_t(a) { if (a) return {}; else impossible; }
auto conj_intro(bool a, bool b, 0 true_t(a), 0 true_t(b)) -> true_t(a and b)
{
    if (a)  if (b) return {};   else impossible;
    else    impossible;
}
auto conj_elim_a(bool a, bool b, 0 true_t(a and b)) -> true_t(a)
{
    if (a)  return {};
    else    if (b) impossible;  else impossible;
}
auto conj_elim_b(bool a, bool b, 0 true_t(a and b)) -> true_t(b)
{
    if (b)  return {};
    else    if (a) impossible;  else impossible;
}
auto disj_intro_a(bool a, bool b, 0 true_t(a)) -> true_t(a or b)
{
    if (a)  if (b) return {};   else return {};
    else    impossible;
}
auto disj_intro_b(bool a, bool b, 0 true_t(b)) -> true_t(a or b)
{
    if (b)  if (a) return {};   else return {};
    else    impossible;
}
auto de_morgan_a(bool a, bool b, 0 true_t(not (a and b))) -> true_t(not a or not b)
{
    if (a)  if (b) impossible;  else return {};
    else    if (b) return {};   else return {};
}
auto de_morgan_b(bool a, bool b, 0 true_t(not (a or b))) -> true_t(not a and not b)
{
    if (a)  if (b) impossible;  else impossible;
    else    if (b) impossible;   else return {};
}
auto modus_ponens(bool a, bool b, 0 true_t(a) p, (0 true_t(a)) -> true_t(b) f) -> true_t(b)
{
    if (a)
        return f(p);
    else
        impossible;
}
auto modus_tollens(bool a, bool b, 0 true_t(not b) q, (0 true_t(a)) -> true_t(b) f) -> true_t(not a)
{
    if (a)
        impossible because contradiction(b, f({}), q);
    else
        return {};
}
auto conjunctive_syllogism_a(bool a, bool b, 0 true_t(not (a and b)), 0 true_t(b)) -> true_t(not a)
{
    if (b)  if (a) impossible;  else return {};
    else    if (a) impossible;  else return {};
}
auto conjunctive_syllogism_b(bool a, bool b, 0 true_t(not (a and b)), 0 true_t(a)) -> true_t(not b)
{
    if (a)  if (b) impossible;  else return {};
    else    if (b) impossible;  else return {};
}
auto disjunctive_syllogism_a(bool a, bool b, 0 true_t(a or b), 0 true_t(not b)) -> true_t(a)
{
    if (a)  return {};
    else    if (b) impossible;  else impossible;
}
auto disjunctive_syllogism_b(bool a, bool b, 0 true_t(a or b), 0 true_t(not a)) -> true_t(b)
{
    if (b)  return {};
    else    if (a) impossible;  else impossible;
}
auto dilemma(
    bool a, bool b, bool c,
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
auto reductio_ad_absurdum(bool a, (0 true_t(a)) -> true_t(false) f) -> true_t(not a)
{
    if (a)  return f({});
    else    return {};
}
auto material_implication(bool a, bool b, (0 true_t(a)) -> true_t(b) f) -> true_t(not a or b)
{
    if (a)  if (b)  return {}; else impossible because f({});
    else    if (b)  return {}; else return {};
}

// bool (==)
auto eq_refl_bool(bool a) -> true_t(a == a) { if (a) return {}; else return {}; }
auto eq_symm_bool(bool a, bool b, 0 true_t(a == b)) -> true_t(b == a)
{
    if (a)  if (b) return {};   else impossible;
    else    if (b) impossible;  else return {};
}
auto eq_trans_bool(bool a, bool b, bool c, 0 true_t(a == b), 0 true_t(b == c)) -> true_t(a == c)
{
    if (a)
        if (b)  if (c) return {};   else impossible;
        else    if (a) impossible;  else impossible;
    else
        if (b)  if (c) impossible;  else impossible;
        else    if (c) impossible;  else return {};
}
auto eq_true_intro(bool a, 0 true_t(a)) -> true_t(a == true) { if (a) return {}; else impossible; }
auto eq_true_elim(bool a, 0 true_t(a == true)) -> true_t(a) { if (a) return {}; else impossible; }
auto eq_false_intro(bool a, 0 true_t(not a)) -> true_t(a == false) { if (a) impossible; else return {}; }
auto eq_false_elim(bool a, 0 true_t(a == false)) -> true_t(not a) { if (a) impossible; else return {}; }
auto not_eq_intro(bool a, bool b, 0 true_t(a != b)) -> true_t(not (a == b))
{
    if (a)  if (b) impossible;  else return {};
    else    if (b) return {};   else impossible;
}
auto not_eq_elim(bool a, bool b, 0 true_t(not (a == b))) -> true_t(a != b)
{
    if (a)  if (b) impossible;  else return {};
    else    if (b) return {};   else impossible;
}
auto not_eq_true_intro(bool a, 0 true_t(not a)) -> true_t(not (a == true)) { if (a) impossible; else return {}; }
auto not_eq_true_elim(bool a, 0 true_t(not (a == true))) -> true_t(not a) { if (a) impossible; else return {}; }
auto not_eq_false_intro(bool a, 0 true_t(a)) -> true_t(not (a == false)) { if (a) return {}; else impossible; }
auto not_eq_false_elim(bool a, 0 true_t(not (a == false))) -> true_t(a) { if (a) return {}; else impossible; }

// bool (!=)
auto neq_symm_bool(bool a, bool b, 0 true_t(a != b)) -> true_t(b != a)
{
    if (a)  if (b) impossible;  else return {};
    else    if (b) return {};   else impossible;
}
auto neq_true_intro(bool a, 0 true_t(not a)) -> true_t(a != true) { if (a) impossible; else return {}; }
auto neq_true_elim(bool a, 0 true_t(a != true)) -> true_t(not a) { if (a) impossible; else return {}; }
auto neq_false_intro(bool a, 0 true_t(a)) -> true_t(a != false) { if (a) return {}; else impossible; }
auto neq_false_elim(bool a, 0 true_t(a != false)) -> true_t(a) { if (a) return {}; else impossible; }
auto not_neq_intro(bool a, bool b, 0 true_t(a == b)) -> true_t(not (a != b))
{
    if (a)  if (b) return {};   else impossible;
    else    if (b) impossible;  else return {};
}
auto not_neq_elim(bool a, bool b, 0 true_t(not (a != b))) -> true_t(a == b)
{
    if (a)  if (b) return {};   else impossible;
    else    if (b) impossible;  else return {};
}
auto not_neq_true_intro(bool a, 0 true_t(a)) -> true_t(not (a != true)) { if (a) return {}; else impossible; }

// bool (<)
auto nothing_smaller_than_false(bool a, 0 true_t(a < false)) -> true_t(false) { if (a) impossible; else impossible; }
auto not_lt_intro(bool a, bool b, 0 true_t(a >= b)) -> true_t(not (a < b))
{
    if (a)  if (b)  return {};  else return {};
    else    if (b)  impossible; else return {};
}
auto not_lt_elim(bool a, bool b, 0 true_t(not (a < b))) -> true_t(a >= b)
{
    if (a)  if (b) return {};   else return {};
    else    if (b) impossible;  else return {};
}
auto a_lt_b_implies_b(bool a, bool b, 0 true_t(a < b)) -> true_t(b)
{
    if (a)  if (b) impossible;  else impossible;
    else    if (b) return {};   else impossible;
}

// bool (<=)
auto lte_refl_bool(bool a) -> true_t(a <= a) { if (a) return {}; else return {}; }
auto lte_trans_bool(bool a, bool b, bool c, 0 true_t(a <= b), 0 true_t(b <= c)) -> true_t(a <= c)
{
    if (a)
        if (b)  if (c) return{};    else impossible;
        else    if (c) impossible;  else impossible;
    else
        if (b)  if (c) return{};    else impossible;
        else    if (c) return{};    else return{};
}
auto true_biggest_bool(bool a) -> true_t(a <= true) { if (a) return {}; else return {}; }
auto false_smallest_bool(bool a) -> true_t(false <= a) { if (a) return {}; else return {}; }

// bool (>)
auto nothing_bigger_than_true(bool a, 0 true_t(a > true)) -> true_t(false) { if (a) impossible; else impossible; }
auto not_gt_intro(bool a, bool b, 0 true_t(a <= b)) -> true_t(not (a > b))
{
    if (a)  if (b) return {};   else impossible;
    else    if (b) return {};   else return {};
}
auto not_gt_elim(bool a, bool b, 0 true_t(not (a > b))) -> true_t(a <= b)
{
    if (a)  if (b) return {};   else impossible;
    else    if (b) return {};   else return {};
}
auto a_gt_b_implies_a(bool a, bool b, 0 true_t(a > b)) -> true_t(a)
{
    if (a)  if (b) impossible; else return {};
    else    if (b) impossible; else impossible;
}

// bool (>=)
auto gte_refl_bool(bool a) -> true_t(a >= a) { if (a) return {}; else return {}; }
auto gte_trans_bool(bool a, bool b, bool c, 0 true_t(a >= b), 0 true_t(b >= c)) -> true_t(a >= c)
{
    if (a)
        if (b)  if (c) return {};   else return {};
        else    if (c) impossible;  else return {};
    else
        if (b)  if (c) impossible;  else impossible;
        else    if (c) impossible;  else return {};
}

auto lt_gt_commute_bool(bool a, bool b, 0 true_t(a < b)) -> true_t(b > a)
{
    if (a)  if (b) impossible;  else impossible;
    else    if (b) return {};   else impossible;
}
auto lte_gte_commute_bool(bool a, bool b, 0 true_t(a <= b)) -> true_t(b >= a)
{
    if (a)  if (b) return {};   else impossible;
    else    if (b) return {};   else return {};
}
auto gt_lt_commute_bool(bool a, bool b, 0 true_t(a > b)) -> true_t(b < a)
{
    if (a)  if (b) impossible;  else return {};
    else    if (b) impossible;  else impossible;
}
auto gte_lte_commute_bool(bool a, bool b, 0 true_t(a >= b)) -> true_t(b <= a)
{
    if (a)  if (b) return {};   else return {};
    else    if (b) impossible;  else return {};
}
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
