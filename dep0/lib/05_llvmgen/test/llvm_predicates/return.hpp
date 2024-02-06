#pragma once

#include "llvm_predicates/predicate.hpp"
#include "llvm_predicates/to_string.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

namespace impl {

template <Predicate<llvm::Value> F>
boost::test_tools::predicate_result is_return_of(llvm::Instruction const& x, F&& f)
{
    auto const ret = llvm::dyn_cast<llvm::ReturnInst>(&x);
    if (not ret)
        return dep0::testing::failure("instruction is not return but: ", x.getOpcodeName());
    auto const v = ret->getReturnValue();
    if (not v)
        return dep0::testing::failure("return instruction has no return value");
    if (auto const result = std::forward<F>(f)(*v); not result)
        return dep0::testing::failure("return value predicate failed: ", result.message());
    return true;
}

template <Predicate<llvm::Value> F>
boost::test_tools::predicate_result is_return_of(llvm::Instruction const* const p, F&& f)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_return_of(*p, std::forward<F>(f));
}

template <Predicate<llvm::Value> F>
boost::test_tools::predicate_result is_return_of(llvm::Value const& x, F&& f)
{
    auto const inst = llvm::dyn_cast<llvm::Instruction>(&x);
    if (not inst)
        return dep0::testing::failure("value is not an instruction but: ", to_string(x));
    return is_return_of(*inst, std::forward<F>(f));
}

template <Predicate<llvm::Value> F>
boost::test_tools::predicate_result is_return_of(llvm::Value const* const p, F&& f)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_return_of(*p, std::forward<F>(f));
}

boost::test_tools::predicate_result is_return_of_void(llvm::Instruction const&);
boost::test_tools::predicate_result is_return_of_void(llvm::Instruction const*);
boost::test_tools::predicate_result is_return_of_void(llvm::Value const&);
boost::test_tools::predicate_result is_return_of_void(llvm::Value const*);

} // namespace impl

inline constexpr auto is_return_of = boost::hana::overload(
    [] <Predicate<llvm::Value> F> (llvm::Instruction const& x, F&& f)
    {
        return impl::is_return_of(x, std::forward<F>(f));
    },
    [] <Predicate<llvm::Value> F> (llvm::Instruction const* const p, F&& f)
    {
        return impl::is_return_of(p, std::forward<F>(f));
    },
    [] <Predicate<llvm::Value> F> (llvm::Value const& x, F&& f)
    {
        return impl::is_return_of(x, std::forward<F>(f));
    },
    [] <Predicate<llvm::Value> F> (llvm::Value const* const p, F&& f)
    {
        return impl::is_return_of(p, std::forward<F>(f));
    }
);

inline constexpr auto is_return_of_void = boost::hana::overload(
    [] (llvm::Instruction const& x) { return impl::is_return_of_void(x); },
    [] (llvm::Instruction const* const p) { return impl::is_return_of_void(p); },
    [] (llvm::Value const& x) { return impl::is_return_of_void(x); },
    [] (llvm::Value const* const p) { return impl::is_return_of_void(p); }
);

} // namespace dep0::llvmgen::testing
