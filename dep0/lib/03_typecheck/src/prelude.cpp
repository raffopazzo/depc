#include "private/prelude.hpp"

#include "dep0/typecheck/check.hpp"

#include "dep0/parser/parse.hpp"
#include "dep0/source.hpp"

namespace dep0::typecheck {

namespace {
auto constexpr prelude_text = R"(
// bool
axiom eq_true_intro(bool a, true_t(a)) -> true_t(a == true);
axiom eq_true_elim(bool a, true_t(a == true)) -> true_t(a);
axiom eq_false_intro(bool a, true_t(not a)) -> true_t(a == false);
axiom eq_false_elim(bool a, true_t(a == false)) -> true_t(not a);
axiom not_eq_true_intro(bool a, true_t(not a)) -> true_t(not (a == true));
axiom not_eq_true_elim(bool a, true_t(not (a == true))) -> true_t(not a);
axiom not_eq_false_intro(bool a, true_t(a)) -> true_t(not (a == false));
axiom not_eq_false_elim(bool a, true_t(not (a == false))) -> true_t(a);

axiom conj_intro(bool a, bool b, true_t(a), true_t(b)) -> true_t(a and b);
axiom conj_elim_a(bool a, bool b, true_t(a and b)) -> true_t(a);
axiom conj_elim_b(bool a, bool b, true_t(a and b)) -> true_t(b);
axiom disj_intro_a(bool a, bool b, true_t(a)) -> true_t(a or b);
axiom disj_intro_b(bool a, bool b, true_t(b)) -> true_t(a or b);
axiom double_neg_intro(bool a, true_t(a)) -> true_t(not not a);
axiom double_neg_elim(bool a, true_t(not not a)) -> true_t(a);

axiom nothing_smaller_than_false(bool a, true_t(a < false)) -> true_t(false);
axiom nothing_bigger_than_true(bool a, true_t(a > true)) -> true_t(false);

auto a_lt_b_implies_b(bool a, bool b, 0 true_t(a < b)) -> true_t(b)
{
    if (b)
        return {};
    else if (a < false)
        impossible because nothing_smaller_than_false(a, auto);
    else
        impossible;
}

auto a_gt_b_implies_a(bool a, bool b, 0 true_t(a > b)) -> true_t(a)
{
    if (b)
        impossible because nothing_bigger_than_true(a, auto);
    else if (a)
        return {};
    else
        impossible;
}

// axiom gt_implies_lt_64(u64_t a, u64_t b, true_t(a > b)) -> true_t(b < a);
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
