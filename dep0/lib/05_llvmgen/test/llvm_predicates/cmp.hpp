/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "llvm_predicates/predicate.hpp"
#include "llvm_predicates/to_string.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Instructions.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

namespace impl {

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
boost::test_tools::predicate_result
    is_cmp(llvm::Instruction const& x, llvm::CmpInst::Predicate const op, F1&& f1, F2&& f2)
{
    using namespace dep0::testing;
    auto const cmp = llvm::dyn_cast<llvm::CmpInst>(&x);
    if (not cmp)
        return failure("instruction is not a cmp but: ", to_string(x));
    if (op != cmp->getPredicate())
        return failure(
            "cmp predicate ",
            llvm::CmpInst::getPredicateName(cmp->getPredicate()).str(),
            " != ",
            llvm::CmpInst::getPredicateName(op).str());
    if (auto const result = std::forward<F1>(f1)(*cmp->getOperand(0)); not result)
        return failure("cmp operand 0 predicate failed: ", result.message());
    if (auto const result = std::forward<F2>(f2)(*cmp->getOperand(1)); not result)
        return failure("cmp operand 1 predicate failed: ", result.message());
    return true;
}

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
boost::test_tools::predicate_result
    is_cmp(llvm::Instruction const* const p, llvm::CmpInst::Predicate const op, F1&& f1, F2&& f2)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_cmp(*p, op, std::forward<F1>(f1), std::forward<F2>(f2));
}

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
boost::test_tools::predicate_result
    is_cmp(llvm::Value const& x, llvm::CmpInst::Predicate const op, F1&& f1, F2&& f2)
{
    auto const i = llvm::dyn_cast<llvm::Instruction>(&x);
    if (not i)
        return dep0::testing::failure("value is not an instruction but: ", to_string(x));
    return is_cmp(*i, op, std::forward<F1>(f1), std::forward<F2>(f2));
}

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
boost::test_tools::predicate_result
    is_call(llvm::Value const* const p, llvm::CmpInst::Predicate const op, F1&& f1, F2&& f2)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_cmp(*p, op, std::forward<F1>(f1), std::forward<F2>(f2));
}

} // namespace impl

inline constexpr auto is_cmp = boost::hana::overload(
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Instruction const& x, llvm::CmpInst::Predicate const op, F1&& f1, F2&& f2)
    {
        return impl::is_cmp(x, op, std::forward<F1>(f1), std::forward<F2>(f2));
    },
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Instruction const* const p, llvm::CmpInst::Predicate const op, F1&& f1, F2&& f2)
    {
        return impl::is_cmp(p, op, std::forward<F1>(f1), std::forward<F2>(f2));
    },
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Value const& x, llvm::CmpInst::Predicate const op, F1&& f1, F2&& f2)
    {
        return impl::is_cmp(x, op, std::forward<F1>(f1), std::forward<F2>(f2));
    },
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Value const* const p, llvm::CmpInst::Predicate const op, F1&& f1, F2&& f2)
    {
        return impl::is_cmp(p, op, std::forward<F1>(f1), std::forward<F2>(f2));
    }
);

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
auto cmp_of(llvm::CmpInst::Predicate const op, F1&& f1, F2&& f2)
{
    struct predicate_t
    {
        llvm::CmpInst::Predicate op;
        std::remove_cvref_t<F1> f1;
        std::remove_cvref_t<F2> f2;

        boost::test_tools::predicate_result operator()(llvm::Instruction const& x) const
        {
            return is_cmp(x, op, f1, f2);
        }

        boost::test_tools::predicate_result operator()(llvm::Value const& x) const
        {
            return is_cmp(x, op, f1, f2);
        }
    };
    return predicate_t{op, std::forward<F1>(f1), std::forward<F2>(f2)};
}

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
auto cmp_eq(F1&& f1, F2&& f2)
{
    return cmp_of(llvm::CmpInst::Predicate::ICMP_EQ, std::forward<F1>(f1), std::forward<F2>(f2));
}

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
auto cmp_ne(F1&& f1, F2&& f2)
{
    return cmp_of(llvm::CmpInst::Predicate::ICMP_NE, std::forward<F1>(f1), std::forward<F2>(f2));
}

} // namespace dep0::llvmgen::testing
