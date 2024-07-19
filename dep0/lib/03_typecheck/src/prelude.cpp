#include "private/prelude.hpp"

#include "dep0/typecheck/check.hpp"

#include "dep0/parser/parse.hpp"
#include "dep0/source.hpp"

namespace dep0::typecheck {

namespace {
auto constexpr prelude_text = R"(
// bool (first order logic)
auto contradiction(bool a, 0 true_t(a), 0 true_t(not a)) -> true_t(false) { if (a) impossible; else impossible; }
axiom double_neg_intro(bool a, true_t(a)) -> true_t(not not a);
axiom double_neg_elim(bool a, true_t(not not a)) -> true_t(a);
axiom conj_intro(bool a, bool b, true_t(a), true_t(b)) -> true_t(a and b);
axiom conj_elim_a(bool a, bool b, true_t(a and b)) -> true_t(a);
axiom conj_elim_b(bool a, bool b, true_t(a and b)) -> true_t(b);
axiom disj_intro_a(bool a, bool b, true_t(a)) -> true_t(a or b);
axiom disj_intro_b(bool a, bool b, true_t(b)) -> true_t(a or b);

// bool (==)
axiom eq_refl_bool(bool a) -> true_t(a == a);
axiom eq_symm_bool(bool a, bool b, true_t(a == b)) -> true_t(b == a);
axiom eq_trans_bool(bool a, bool b, bool c, true_t(a == b), true_t(b == c)) -> true_t(a == c);
auto eq_true_intro(bool a, 0 true_t(a)) -> true_t(a == true) { if (a) return {}; else impossible; }
auto eq_true_elim(bool a, 0 true_t(a == true)) -> true_t(a) { if (a) return {}; else impossible; }
auto eq_false_intro(bool a, 0 true_t(not a)) -> true_t(a == false) { if (a) impossible; else return {}; }
auto eq_false_elim(bool a, 0 true_t(a == false)) -> true_t(not a) { if (a) impossible; else return {}; }
axiom not_eq_intro(bool a, bool b, true_t(a != b)) -> true_t(not (a == b));
axiom not_eq_elim(bool a, bool b, true_t(not (a == b))) -> true_t(a != b);
auto not_eq_true_intro(bool a, 0 true_t(not a)) -> true_t(not (a == true)) { if (a) impossible; else return {}; }
auto not_eq_true_elim(bool a, 0 true_t(not (a == true))) -> true_t(not a) { if (a) impossible; else return {}; }
auto not_eq_false_intro(bool a, 0 true_t(a)) -> true_t(not (a == false)) { if (a) return {}; else impossible; }
auto not_eq_false_elim(bool a, 0 true_t(not (a == false))) -> true_t(a) { if (a) return {}; else impossible; }

// bool (!=)
axiom neq_symm_bool(bool a, bool b, true_t(a != b)) -> true_t(b != a);
auto neq_true_intro(bool a, 0 true_t(not a)) -> true_t(a != true) { if (a) impossible; else return {}; }
auto neq_true_elim(bool a, 0 true_t(a != true)) -> true_t(not a) { if (a) impossible; else return {}; }
auto neq_false_intro(bool a, 0 true_t(a)) -> true_t(a != false) { if (a) return {}; else impossible; }
auto neq_false_elim(bool a, 0 true_t(a != false)) -> true_t(a) { if (a) return {}; else impossible; }
axiom not_neq_intro(bool a, bool b, true_t(a == b)) -> true_t(not (a != b));
axiom not_neq_elim(bool a, bool b, true_t(not (a != b))) -> true_t(a == b);
auto not_neq_true_intro(bool a, 0 true_t(a)) -> true_t(not (a != true)) { if (a) return {}; else impossible; }

// bool (<)
axiom nothing_smaller_than_false(bool a, true_t(a < false)) -> true_t(false);
axiom not_lt_intro(bool a, bool b, true_t(a >= b)) -> true_t(not (a < b));
axiom not_lt_elim(bool a, bool b, true_t(not (a < b))) -> true_t(a >= b);
auto a_lt_b_implies_b(bool a, bool b, 0 true_t(a < b)) -> true_t(b)
{
    if (b)
        return {};
    else if (a < false)
        impossible because nothing_smaller_than_false(a, auto);
    else
        impossible;
}

// bool (<=)
axiom lte_refl_bool(bool a) -> true_t(a <= a);
axiom lte_tran_bool(bool a, bool b, bool c, true_t(a <= b), true_t(b <= c)) -> true_t(a <= c);
axiom true_biggest_bool(bool a) -> true_t(a <= true);
axiom false_smallest_bool(bool a) -> true_t(false <= a);

// bool (>)
axiom nothing_bigger_than_true(bool a, true_t(a > true)) -> true_t(false);
axiom not_gt_intro(bool a, bool b, true_t(a <= b)) -> true_t(not (a > b));
axiom not_gt_elim(bool a, bool b, true_t(not (a > b))) -> true_t(a <= b);
auto a_gt_b_implies_a(bool a, bool b, 0 true_t(a > b)) -> true_t(a)
{
    if (b)
        impossible because nothing_bigger_than_true(a, auto);
    else if (a)
        return {};
    else
        impossible;
}

// bool (>=)
axiom gte_refl_bool(bool a) -> true_t(a >= a);
axiom gte_tran_bool(bool a, bool b, bool c, true_t(a >= b), true_t(b >= c)) -> true_t(a >= c);

axiom lt_gt_commute_bool(bool a, bool b, true_t(a < b)) -> true_t(b > a);
axiom lte_gte_commute_bool(bool a, bool b, true_t(a <= b)) -> true_t(b >= a);
axiom gt_lt_commute_bool(bool a, bool b, true_t(a > b)) -> true_t(b < a);
axiom gte_lte_commute_bool(bool a, bool b, true_t(a >= b)) -> true_t(b <= a);
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
