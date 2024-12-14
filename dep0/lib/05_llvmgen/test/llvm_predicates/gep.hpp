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

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Value>... F_idx>
boost::test_tools::predicate_result is_gep_of(llvm::Instruction const& x, F_type&& f_type, F_ptr&& f_ptr, F_idx&&... f_idx)
{
    using namespace dep0::testing;
    auto const gep = llvm::dyn_cast<llvm::GetElementPtrInst>(&x);
    if (not gep)
        return failure("instruction is not getelementptr but: ", x.getOpcodeName());
    if (llvm::GetElementPtrInst::getPointerOperandIndex() != 0)
        return failure("LLVM changed the operand index from 0 to ", llvm::GetElementPtrInst::getPointerOperandIndex());
    if (auto const result = std::forward<F_type>(f_type)(*gep->getSourceElementType()); not result)
        return failure("GEP source element type predicate failed: ", result.message());
    if (auto const result = std::forward<F_ptr>(f_ptr)(*gep->getPointerOperand()); not result)
        return failure("GEP operand predicate failed: ", result.message());
    auto constexpr N = sizeof...(F_idx);
    if (auto const n_indices = gep->getNumIndices(); n_indices != N)
        return failure("wrong number of indices: ", N, " != ", n_indices);
    auto result = boost::test_tools::predicate_result(true);
    int next = 1; // operand 0 is the base pointer not an index
    ([&]
    {
        if (result)
        {
            auto const i = next++;
            if (auto const tmp = std::forward<F_idx>(f_idx)(*gep->getOperand(i)); not tmp)
                result = failure("GEP index ", i, ": ", tmp.message());
        }
    }(), ...);
    return result;
}

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Value>... F_idx>
boost::test_tools::predicate_result
    is_gep_of(llvm::Instruction const* const p, F_type&& f_type, F_ptr&& f_ptr, F_idx&&... f_idx)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_gep_of(*p, std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_idx>(f_idx)...);
}

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Value>... F_idx>
boost::test_tools::predicate_result is_gep_of(llvm::Value const& x, F_type&& f_type, F_ptr&& f_ptr, F_idx&&... f_idx)
{
    auto const instr = llvm::dyn_cast<llvm::Instruction>(&x);
    if (not instr)
        return dep0::testing::failure("value is not instruction but: ", to_string(x));
    return is_gep_of(*instr, std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_idx>(f_idx)...);
}

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Value>... F_idx>
boost::test_tools::predicate_result
    is_gep_of(llvm::Value const* const p, F_type&& f_type, F_ptr&& f_ptr, F_idx&&... f_idx)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_gep_of(*p, std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_idx>(f_idx)...);
}

} // namespace impl

inline constexpr auto is_gep_of = boost::hana::overload(
    [] <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Value>... F_idx>
    (llvm::Instruction const& x, F_type&& f_type, F_ptr&& f_ptr, F_idx&&... f_idx)
    {
        return impl::is_gep_of(x, std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_idx>(f_idx)...);
    },
    [] <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Value>... F_idx>
    (llvm::Instruction const* const p, F_type&& f_type, F_ptr&& f_ptr, F_idx&&... f_idx)
    {
        return impl::is_gep_of(p, std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_idx>(f_idx)...);
    },
    [] <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Value>... F_idx>
    (llvm::Value const& x, F_type&& f_type, F_ptr&& f_ptr, F_idx&&... f_idx)
    {
        return impl::is_gep_of(x, std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_idx>(f_idx)...);
    },
    [] <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Value>... F_idx>
    (llvm::Value const* const p, F_type&& f_type, F_ptr&& f_ptr, F_idx&&... f_idx)
    {
        return impl::is_gep_of(p, std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_idx>(f_idx)...);
    }
);

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Value>... F_idx>
auto gep_of(F_type&& f_type, F_ptr&& f_ptr, F_idx&&... f_idx)
{
    struct predicate_t
    {
        std::remove_cvref_t<F_type> f_type;
        std::remove_cvref_t<F_ptr> f_ptr;
        std::tuple<std::remove_cvref_t<F_idx>...> f_idx;

        boost::test_tools::predicate_result operator()(llvm::Instruction const& x) const
        {
            return std::apply(
                is_gep_of,
                [&] <std::size_t... Is> (std::index_sequence<Is...>)
                {
                    return std::forward_as_tuple(x, f_type, f_ptr, std::get<Is>(f_idx)...);
                }(std::make_index_sequence<sizeof...(F_idx)>{}));
        }

        boost::test_tools::predicate_result operator()(llvm::Value const& x) const
        {
            return std::apply(
                is_gep_of,
                [&] <std::size_t... Is> (std::index_sequence<Is...>)
                {
                    return std::forward_as_tuple(x, f_type, f_ptr, std::get<Is>(f_idx)...);
                }(std::make_index_sequence<sizeof...(F_idx)>{}));
        }
    };
    return predicate_t{std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), {std::forward<F_idx>(f_idx)...}};
}

} // namespace dep0::llvmgen::testing
