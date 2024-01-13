#pragma once

#include "llvm_predicates_v2/predicate.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing::v2 {

namespace impl {

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
boost::test_tools::predicate_result is_add_of(llvm::Instruction const& x, F1&& f1, F2&& f2)
{
    auto const op = llvm::dyn_cast<llvm::BinaryOperator>(&x);
    if (not op)
        return dep0::testing::failure("instruction is not a binary operator");
    if (op->getOpcode() != llvm::Instruction::BinaryOps::Add)
        return dep0::testing::failure(
            "binary operator is not Add: ",
            static_cast<int>(llvm::Instruction::BinaryOps::Add),
            " != ",
            static_cast<int>(op->getOpcode()));
    if (op->getNumOperands() != 2)
        return dep0::testing::failure("addition does not have 2 operands but ", op->getNumOperands());
    if (auto const result = std::forward<F1>(f1)(*op->getOperand(0)); not result)
        return dep0::testing::failure("predicate has failed for 1st operand: ", result.message());
    if (auto const result = std::forward<F2>(f2)(*op->getOperand(1)); not result)
        return dep0::testing::failure("predicate has failed for 2nd operand: ", result.message());
    return true;
}

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
boost::test_tools::predicate_result is_add_of(llvm::Instruction const* const p, F1&& f1, F2&& f2)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_add_of(*p, std::forward<F1>(f1), std::forward<F2>(f2));
}

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
boost::test_tools::predicate_result is_add_of(llvm::Value const& x, F1&& f1, F2&& f2)
{
    auto const p = llvm::dyn_cast<llvm::Instruction>(&x);
    if (not p)
        return dep0::testing::failure("value is not an instruction");
    return is_add_of(*p, std::forward<F1>(f1), std::forward<F2>(f2));
}

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
boost::test_tools::predicate_result is_add_of(llvm::Value const* const p, F1&& f1, F2&& f2)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_add_of(*p, std::forward<F1>(f1), std::forward<F2>(f2));
}

} // namespace impl

inline constexpr auto is_add_of = boost::hana::overload(
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Instruction const& x, F1&& f1, F2&& f2)
    {
        return impl::is_add_of(x, std::forward<F1>(f1), std::forward<F2>(f2));
    },
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Instruction const* const p, F1&& f1, F2&& f2)
    {
        return impl::is_add_of(p, std::forward<F1>(f1), std::forward<F2>(f2));
    },
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Value const& x, F1&& f1, F2&& f2)
    {
        return impl::is_add_of(x, std::forward<F1>(f1), std::forward<F2>(f2));
    },
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Value const* const p, F1&& f1, F2&& f2)
    {
        return impl::is_add_of(p, std::forward<F1>(f1), std::forward<F2>(f2));
    }
);

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
auto add_of(F1&& f1, F2&& f2)
{
    struct predicate_t
    {
        std::remove_cvref_t<F1> f1;
        std::remove_cvref_t<F2> f2;

        boost::test_tools::predicate_result operator()(llvm::Instruction const& x) const
        {
            return impl::is_add_of(x, f1, f2);
        }

        boost::test_tools::predicate_result operator()(llvm::Value const& x) const
        {
            return impl::is_add_of(x, f1, f2);
        }
    };
    return predicate_t{std::forward<F1>(f1), std::forward<F2>(f2)};
}

} // namespace dep0::llvmgen::testing
