/*
 * Copyright Raffaele Rossi 2025.
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

template <Predicate<llvm::Type> F_ty, Predicate<llvm::Value>... F>
boost::test_tools::predicate_result
    is_phi_of(llvm::Instruction const& x, F_ty&& f_ty, std::pair<F, llvm::BasicBlock const*>... f)
{
    using namespace dep0::testing;
    auto const phi = llvm::dyn_cast<llvm::PHINode>(&x);
    if (not phi)
        return failure("instruction is not a phi but: ", to_string(x));
    if (auto const tmp = std::forward<F_ty>(f_ty)(*phi->getType()); not tmp)
        return failure("inside phi type: ", tmp.message());
    auto constexpr N = sizeof...(F);
    if (auto const n_values = phi->getNumIncomingValues(); n_values != N)
        return failure("wrong number of incoming values ", N, " != ", n_values);
    auto result = boost::test_tools::predicate_result(true);
    int next = 0;
    ([&]
    {
        if (result)
        {
            auto const i = next++;
            auto const value = phi->getIncomingValue(i);
            if (not value)
            {
                result = failure("incoming value ", i, " is null");
                return;
            }
            if (auto const tmp = f.first(*value); not tmp)
            {
                result = failure("inside incoming value ", i, ": ", tmp.message());
                return;
            }
            if (auto const block = phi->getIncomingBlock(i); block != f.second)
            {
                result = failure(
                    "incoming block ", i, " ",
                    block, '(', block->getName().str(), ") != ",
                    f.second, '(', f.second->getName().str(), ')');
                return;
            }
        }
    }(), ...);
    return result;
}

template <Predicate<llvm::Type> F_ty, Predicate<llvm::Value>... F>
boost::test_tools::predicate_result
    is_phi_of(llvm::Instruction const* const p, F_ty&& f_ty, std::pair<F, llvm::BasicBlock const*>... f)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_phi_of(*p, std::forward<F_ty>(f_ty), std::move(f)...);
}

template <Predicate<llvm::Type> F_ty, Predicate<llvm::Value>... F>
boost::test_tools::predicate_result
    is_phi_of(llvm::Value const& x, F_ty&& f_ty, std::pair<F, llvm::BasicBlock const*>... f)
{
    auto const i = llvm::dyn_cast<llvm::Instruction>(&x);
    if (not i)
        return dep0::testing::failure("value is not an instruction but: ", to_string(x));
    return is_phi_of(i, std::forward<F_ty>(f_ty), std::move(f)...);
}

template <Predicate<llvm::Type> F_ty, Predicate<llvm::Value>... F>
boost::test_tools::predicate_result
    is_phi_of(llvm::Value const* const p, F_ty&& f_ty, std::pair<F, llvm::BasicBlock const*>... f)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_phi_of(*p, std::forward<F_ty>(f_ty), std::move(f)...);
}

} // namespace impl

inline constexpr auto is_phi_of = boost::hana::overload(
    [] <Predicate<llvm::Type> F_ty, Predicate<llvm::Value>... F>
    (llvm::Instruction const& x, F_ty&& f_ty, std::pair<F, llvm::BasicBlock const*>... f)
    {
        return impl::is_phi_of(x, std::forward<F_ty>(f_ty), std::move(f)...);
    },
    [] <Predicate<llvm::Type> F_ty, Predicate<llvm::Value>... F>
    (llvm::Instruction const* const p, F_ty&& f_ty, std::pair<F, llvm::BasicBlock const*>... f)
    {
        return impl::is_phi_of(p, std::forward<F_ty>(f_ty), std::move(f)...);
    },
    [] <Predicate<llvm::Type> F_ty, Predicate<llvm::Value>... F>
    (llvm::Value const& x, F_ty&& f_ty, std::pair<F, llvm::BasicBlock const*>... f)
    {
        return impl::is_phi_of(x, std::forward<F_ty>(f_ty), std::move(f)...);
    },
    [] <Predicate<llvm::Type> F_ty, Predicate<llvm::Value>... F>
    (llvm::Value const* const p, F_ty&& f_ty, std::pair<F, llvm::BasicBlock const*>... f)
    {
        return impl::is_phi_of(p, std::forward<F_ty>(f_ty), std::move(f)...);
    }
);

template <Predicate<llvm::Type> F_ty, Predicate<llvm::Value>... F>
auto phi_of(F_ty&& f_ty, std::pair<F, llvm::BasicBlock const*>... f)
{
    struct predicate_t
    {
        std::remove_cvref_t<F_ty> f_ty;
        std::tuple<std::pair<F, llvm::BasicBlock const*>...> f;

        boost::test_tools::predicate_result operator()(llvm::Instruction const& x) const
        {
            return std::apply(is_phi_of, std::tuple_cat(std::tie(x, f_ty), f));
        }

        boost::test_tools::predicate_result operator()(llvm::Value const& x) const
        {
            return std::apply(is_phi_of, std::tuple_cat(std::tie(x, f_ty), f));
        }
    };
    return predicate_t{std::forward<F_ty>(f_ty), std::move(f)...};
}

} // namespace dep0::llvmgen::testing
