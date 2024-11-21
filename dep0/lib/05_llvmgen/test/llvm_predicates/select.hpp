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

template <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_true, Predicate<llvm::Value> F_false>
boost::test_tools::predicate_result
    is_select_of(llvm::Instruction const& x, F_cond&& f_cond, F_true&& f_true, F_false&& f_false)
{
    using namespace dep0::testing;
    auto const select = llvm::dyn_cast<llvm::SelectInst>(&x);
    if (not select)
        return failure("instruction is not a select but: ", to_string(x));
    if (auto const result = std::forward<F_cond>(f_cond)(*select->getCondition()); not result)
        return failure("select condition predicate failed: ", result.message());
    if (auto const result = std::forward<F_true>(f_true)(*select->getTrueValue()); not result)
        return failure("select true value predicate failed: ", result.message());
    if (auto const result = std::forward<F_false>(f_false)(*select->getFalseValue()); not result)
        return failure("select false value predicate failed: ", result.message());
    return boost::test_tools::predicate_result(true);
}

template <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_true, Predicate<llvm::Value> F_false>
boost::test_tools::predicate_result
    is_select_of(llvm::Instruction const* const p, F_cond&& f_cond, F_true&& f_true, F_false&& f_false)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_select_of(*p, std::forward<F_cond>(f_cond), std::forward<F_true>(f_true), std::forward<F_false>(f_false));
}

template <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_true, Predicate<llvm::Value> F_false>
boost::test_tools::predicate_result
    is_select_of(llvm::Value const& x, F_cond&& f_cond, F_true&& f_true, F_false&& f_false)
{
    auto const i = llvm::dyn_cast<llvm::Instruction>(&x);
    if (not i)
        return dep0::testing::failure("value is not an instruction but: ", to_string(x));
    return is_select_of(*i, std::forward<F_cond>(f_cond), std::forward<F_true>(f_true), std::forward<F_false>(f_false));
}

template <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_true, Predicate<llvm::Value> F_false>
boost::test_tools::predicate_result
    is_select_of(llvm::Value const* const p, F_cond&& f_cond, F_true&& f_true, F_false&& f_false)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_select_of(*p, std::forward<F_cond>(f_cond), std::forward<F_true>(f_true), std::forward<F_false>(f_false));
}

} // namespace impl

inline constexpr auto is_select_of =  boost::hana::overload(
    [] <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_true, Predicate<llvm::Value> F_false>
    (llvm::Instruction const& x, F_cond&& f1, F_true&& f2, F_false&& f3)
    {
        return impl::is_select_of(x, std::forward<F_cond>(f1), std::forward<F_true>(f2), std::forward<F_false>(f3));
    },
    [] <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_true, Predicate<llvm::Value> F_false>
    (llvm::Instruction const* const p, F_cond&& f1, F_true&& f2, F_false&& f3)
    {
        return impl::is_select_of(p, std::forward<F_cond>(f1), std::forward<F_true>(f2), std::forward<F_false>(f3));
    },
    [] <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_true, Predicate<llvm::Value> F_false>
    (llvm::Value const& x, F_cond&& f1, F_true&& f2, F_false&& f3)
    {
        return impl::is_select_of(x, std::forward<F_cond>(f1), std::forward<F_true>(f2), std::forward<F_false>(f3));
    },
    [] <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_true, Predicate<llvm::Value> F_false>
    (llvm::Value const* const p, F_cond&& f1, F_true&& f2, F_false&& f3)
    {
        return impl::is_select_of(p, std::forward<F_cond>(f1), std::forward<F_true>(f2), std::forward<F_false>(f3));
    }
);

template <Predicate<llvm::Value> F_cond, Predicate<llvm::Value> F_true, Predicate<llvm::Value> F_false>
auto select_of(F_cond&& f_cond, F_true&& f_true, F_false&& f_false)
{
    struct predicate_t
    {
        std::remove_cvref_t<F_cond> f_cond;
        std::remove_cvref_t<F_true> f_true;
        std::remove_cvref_t<F_false> f_false;

        boost::test_tools::predicate_result operator()(llvm::Instruction const& x) const
        {
            return is_select_of(x, f_cond, f_true, f_false);
        }

        boost::test_tools::predicate_result operator()(llvm::Value const& x) const
        {
            return is_select_of(x, f_cond, f_true, f_false);
        }
    };
    return predicate_t{std::forward<F_cond>(f_cond), std::forward<F_true>(f_true), std::forward<F_false>(f_false)};
}

} // namespace dep0::llvmgen::testing
