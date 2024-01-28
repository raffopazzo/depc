#pragma once

#include "llvm_predicates_v2/predicate.hpp"
#include "llvm_predicates_v2/to_string.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Instructions.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing::v2 {

namespace impl {

template <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_then, Predicate<llvm::Value> F_else>
boost::test_tools::predicate_result
    is_branch_of(llvm::Instruction const& x, F_cond&& f_cond, F_then&& f_then, F_else&& f_else)
{
    using namespace dep0::testing;
    auto const br = llvm::dyn_cast<llvm::BranchInst>(&x);
    if (not br)
        return failure("instruction is not a branch but: ", to_string(x));
    if (not br->isConditional())
        return failure("branch is not conditional");
    if (auto const result = std::forward<F_cond>(f_cond)(*br->getCondition()); not result)
        return failure("branch condition predicate failed: ", result.message());
    if (auto const n = br->getNumSuccessors(); n != 2ul)
        return failure("branch should have 2 successor blocks but has: ", n);
    if (auto const result = std::forward<F_then>(f_then)(*br->getSuccessor(0)); not result)
        return failure("branch successor 0 predicate failed: ", result.message());
    if (auto const result = std::forward<F_else>(f_else)(*br->getSuccessor(1)); not result)
        return failure("branch successor 1 predicate failed: ", result.message());
    return boost::test_tools::predicate_result(true);
}

template <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_then, Predicate<llvm::Value> F_else>
boost::test_tools::predicate_result
    is_branch_of(llvm::Instruction const* const p, F_cond&& f_cond, F_then&& f_then, F_else&& f_else)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_branch_of(*p, std::forward<F_cond>(f_cond), std::forward<F_then>(f_then), std::forward<F_else>(f_else));
}

template <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_then, Predicate<llvm::Value> F_else>
boost::test_tools::predicate_result
    is_branch_of(llvm::Value const& x, F_cond&& f_cond, F_then&& f_then, F_else&& f_else)
{
    auto const i = llvm::dyn_cast<llvm::Instruction>(&x);
    if (not i)
        return dep0::testing::failure("value is not an instruction but: ", to_string(x));
    return is_branch_of(*i, std::forward<F_cond>(f_cond), std::forward<F_then>(f_then), std::forward<F_else>(f_else));
}

template <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_then, Predicate<llvm::Value> F_else>
boost::test_tools::predicate_result
    is_branch_of(llvm::Value const* const p, F_cond&& f_cond, F_then&& f_then, F_else&& f_else)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_branch_of(*p, std::forward<F_cond>(f_cond), std::forward<F_then>(f_then), std::forward<F_else>(f_else));
}

template <Predicate<llvm::Value> F>
boost::test_tools::predicate_result is_unconditional_branch_to(llvm::Instruction const& x, F&& f)
{
    using namespace dep0::testing;
    auto const br = llvm::dyn_cast<llvm::BranchInst>(&x);
    if (not br)
        return failure("instruction is not a branch but: ", to_string(x));
    if (br->isConditional())
        return failure("branch should be unconditional but it is not");
    if (auto const n = br->getNumSuccessors(); n != 1ul)
        return failure("branch should have 1 successor block but has: ", n);
    if (auto const result = std::forward<F>(f)(*br->getSuccessor(0)); not result)
        return failure("branch successor predicate failed: ", result.message());
    return boost::test_tools::predicate_result(true);
}

template <Predicate<llvm::Value> F>
boost::test_tools::predicate_result is_unconditional_branch_to(llvm::Instruction const* const p, F&& f)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_unconditional_branch_to(*p, std::forward<F>(f));
}

template <Predicate<llvm::Value> F>
boost::test_tools::predicate_result is_unconditional_branch_to(llvm::Value const& x, F&& f)
{
    auto const i = llvm::dyn_cast<llvm::Instruction>(&x);
    if (not i)
        return dep0::testing::failure("value is not an instruction but: ", to_string(x));
    return is_unconditional_branch_to(*i, std::forward<F>(f));
}

template <Predicate<llvm::Value> F>
boost::test_tools::predicate_result is_unconditional_branch_to(llvm::Value const* const p, F&& f)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_unconditional_branch_to(*p, std::forward<F>(f));
}

} // namespace impl

inline constexpr auto is_branch_of =  boost::hana::overload(
    [] <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_then, Predicate<llvm::Value> F_else>
    (llvm::Instruction const& x, F_cond&& f1, F_then&& f2, F_else&& f3)
    {
        return impl::is_branch_of(x, std::forward<F_cond>(f1), std::forward<F_then>(f2), std::forward<F_else>(f3));
    },
    [] <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_then, Predicate<llvm::Value> F_else>
    (llvm::Instruction const* const p, F_cond&& f1, F_then&& f2, F_else&& f3)
    {
        return impl::is_branch_of(p, std::forward<F_cond>(f1), std::forward<F_then>(f2), std::forward<F_else>(f3));
    },
    [] <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_then, Predicate<llvm::Value> F_else>
    (llvm::Value const& x, F_cond&& f1, F_then&& f2, F_else&& f3)
    {
        return impl::is_branch_of(x, std::forward<F_cond>(f1), std::forward<F_then>(f2), std::forward<F_else>(f3));
    },
    [] <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_then, Predicate<llvm::Value> F_else>
    (llvm::Value const* const p, F_cond&& f1, F_then&& f2, F_else&& f3)
    {
        return impl::is_branch_of(p, std::forward<F_cond>(f1), std::forward<F_then>(f2), std::forward<F_else>(f3));
    }
);

template <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_then, Predicate<llvm::Value> F_else>
auto branch_of(F_cond&& f_cond, F_then&& f_then, F_else&& f_else)
{
    struct predicate_t
    {
        std::remove_cvref_t<F_cond> f_cond;
        std::remove_cvref_t<F_then> f_then;
        std::remove_cvref_t<F_else> f_else;

        boost::test_tools::predicate_result operator()(llvm::Instruction const& x) const
        {
            return is_branch_of(x, f_cond, f_then, f_else);
        }

        boost::test_tools::predicate_result operator()(llvm::Value const& x) const
        {
            return is_branch_of(x, f_cond, f_then, f_else);
        }
    };
    return predicate_t{std::forward<F_cond>(f_cond), std::forward<F_then>(f_then), std::forward<F_else>(f_else)};
}

inline constexpr auto is_unconditional_branch_to =  boost::hana::overload(
    [] <Predicate<llvm::Value> F> (llvm::Instruction const& x, F&& f)
    {
        return impl::is_unconditional_branch_to(x, std::forward<F>(f));
    },
    [] <Predicate<llvm::Value> F> (llvm::Instruction const* const p, F&& f)
    {
        return impl::is_unconditional_branch_to(p, std::forward<F>(f));
    },
    [] <Predicate<llvm::Value> F> (llvm::Value const& x, F&& f)
    {
        return impl::is_unconditional_branch_to(x, std::forward<F>(f));
    },
    [] <Predicate<llvm::Value> F> (llvm::Value const* const p, F&& f)
    {
        return impl::is_unconditional_branch_to(p, std::forward<F>(f));
    }
);

template <Predicate<llvm::Value> F>
auto unconditional_branch_to(F&& f)
{
    struct predicate_t
    {
        std::remove_cvref_t<F> f;

        boost::test_tools::predicate_result operator()(llvm::Instruction const& x) const
        {
            return is_unconditional_branch_to(x, f);
        }

        boost::test_tools::predicate_result operator()(llvm::Value const& x) const
        {
            return is_unconditional_branch_to(x, f);
        }
    };
    return predicate_t{std::forward<F>(f)};
}

} // namespace dep0::llvmgen::testing::v2
