/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "llvm_predicates/predicate.hpp"
#include "llvm_predicates/to_string.hpp"

#include "dep0/ast/ast.hpp"
#include "dep0/testing/failure.hpp"

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

namespace impl {

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
boost::test_tools::predicate_result is_div_of(llvm::Instruction const& x, ast::sign_t const sign, F1&& f1, F2&& f2)
{
    using enum llvm::Instruction::BinaryOps;
    auto const op = llvm::dyn_cast<llvm::BinaryOperator>(&x);
    if (not op)
        return dep0::testing::failure("instruction is not a binary operator but: ", x.getOpcodeName());
    auto const [opcode, opname] = sign == ast::sign_t::signed_v ? std::pair{SDiv, "SDiv"} : std::pair{UDiv, "UDiv"};
    if (op->getOpcode() != opcode)
        return dep0::testing::failure(
            "binary operator is not ", opname, ": ",
            static_cast<int>(opcode), " != ", static_cast<int>(op->getOpcode()));
    if (op->getNumOperands() != 2)
        return dep0::testing::failure("div does not have 2 operands but ", op->getNumOperands());
    if (auto const result = std::forward<F1>(f1)(*op->getOperand(0)); not result)
        return dep0::testing::failure("predicate has failed for 1st operand: ", result.message());
    if (auto const result = std::forward<F2>(f2)(*op->getOperand(1)); not result)
        return dep0::testing::failure("predicate has failed for 2nd operand: ", result.message());
    return true;
}

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
boost::test_tools::predicate_result is_div_of(llvm::Instruction const* const p, ast::sign_t const s, F1&& f1, F2&& f2)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_div_of(*p, s, std::forward<F1>(f1), std::forward<F2>(f2));
}

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
boost::test_tools::predicate_result is_div_of(llvm::Value const& x, ast::sign_t const s, F1&& f1, F2&& f2)
{
    auto const p = llvm::dyn_cast<llvm::Instruction>(&x);
    if (not p)
        return dep0::testing::failure("value is not an instruction but: ", to_string(x));
    return is_div_of(*p, s, std::forward<F1>(f1), std::forward<F2>(f2));
}

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
boost::test_tools::predicate_result is_div_of(llvm::Value const* const p, ast::sign_t const s, F1&& f1, F2&& f2)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_div_of(*p, s, std::forward<F1>(f1), std::forward<F2>(f2));
}

} // namespace impl

inline constexpr auto is_sdiv_of = boost::hana::overload(
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Instruction const& x, F1&& f1, F2&& f2)
    {
        return impl::is_div_of(x, ast::sign_t::signed_v, std::forward<F1>(f1), std::forward<F2>(f2));
    },
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Instruction const* const p, F1&& f1, F2&& f2)
    {
        return impl::is_div_of(p, ast::sign_t::signed_v, std::forward<F1>(f1), std::forward<F2>(f2));
    },
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Value const& x, F1&& f1, F2&& f2)
    {
        return impl::is_div_of(x, ast::sign_t::signed_v, std::forward<F1>(f1), std::forward<F2>(f2));
    },
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Value const* const p, F1&& f1, F2&& f2)
    {
        return impl::is_div_of(p, ast::sign_t::signed_v, std::forward<F1>(f1), std::forward<F2>(f2));
    }
);

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
auto sdiv_of(F1&& f1, F2&& f2)
{
    struct predicate_t
    {
        std::remove_cvref_t<F1> f1;
        std::remove_cvref_t<F2> f2;

        boost::test_tools::predicate_result operator()(llvm::Instruction const& x) const
        {
            return impl::is_div_of(x, ast::sign_t::signed_v, f1, f2);
        }

        boost::test_tools::predicate_result operator()(llvm::Value const& x) const
        {
            return impl::is_div_of(x, ast::sign_t::signed_v, f1, f2);
        }
    };
    return predicate_t{std::forward<F1>(f1), std::forward<F2>(f2)};
}

inline constexpr auto is_udiv_of = boost::hana::overload(
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Instruction const& x, F1&& f1, F2&& f2)
    {
        return impl::is_div_of(x, ast::sign_t::unsigned_v, std::forward<F1>(f1), std::forward<F2>(f2));
    },
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Instruction const* const p, F1&& f1, F2&& f2)
    {
        return impl::is_div_of(p, ast::sign_t::unsigned_v, std::forward<F1>(f1), std::forward<F2>(f2));
    },
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Value const& x, F1&& f1, F2&& f2)
    {
        return impl::is_div_of(x, ast::sign_t::unsigned_v, std::forward<F1>(f1), std::forward<F2>(f2));
    },
    [] <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
    (llvm::Value const* const p, F1&& f1, F2&& f2)
    {
        return impl::is_div_of(p, ast::sign_t::unsigned_v, std::forward<F1>(f1), std::forward<F2>(f2));
    }
);

template <Predicate<llvm::Value> F1, Predicate<llvm::Value> F2>
auto udiv_of(F1&& f1, F2&& f2)
{
    struct predicate_t
    {
        std::remove_cvref_t<F1> f1;
        std::remove_cvref_t<F2> f2;

        boost::test_tools::predicate_result operator()(llvm::Instruction const& x) const
        {
            return impl::is_div_of(x, ast::sign_t::unsigned_v, f1, f2);
        }

        boost::test_tools::predicate_result operator()(llvm::Value const& x) const
        {
            return impl::is_div_of(x, ast::sign_t::unsigned_v, f1, f2);
        }
    };
    return predicate_t{std::forward<F1>(f1), std::forward<F2>(f2)};
}

} // namespace dep0::llvmgen::testing
