#include "dep0/typecheck/prelude.hpp"

#include "dep0/typecheck/check.hpp"

#include "dep0/parser/parse.hpp"
#include "dep0/source.hpp"

namespace dep0::typecheck {

dep0::expected<module_t> build_prelude_module()
{
    source_text const source = source_text::from_literal(R"(
    // bool
    axiom conj_intro(bool a, bool b, true_t(a), true_t(b)) -> true_t(a and b);
    axiom conj_elim_a(bool a, bool b, true_t(a and b)) -> true_t(a);
    axiom conj_elim_b(bool a, bool b, true_t(a and b)) -> true_t(b);

    // axiom gt_implies_lt_64(u64_t a, u64_t b, true_t(a > b)) -> true_t(b < a);
    )");
    auto const parse_result = parser::parse(source);
    if (not parse_result)
        return dep0::error_t("error parsing prelude module", {std::move(parse_result.error())});
    auto const check_result = check(env_t{}, *parse_result);
    if (not check_result)
        return dep0::error_t("error type-checking prelude module", {std::move(check_result.error())});
    return *check_result;
}

} // namespace dep0::typecheck
