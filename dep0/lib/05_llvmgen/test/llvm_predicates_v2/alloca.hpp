#pragma once

#include "llvm_predicates_v2/predicate.hpp"
#include "llvm_predicates_v2/to_string.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Alignment.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing::v2 {

namespace impl {

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_size, Predicate<llvm::Align> F_align>
boost::test_tools::predicate_result
    is_alloca(llvm::Instruction const& x, F_type&& f_type, F_size&& f_size, F_align&& f_align)
{
    using namespace dep0::testing;
    auto const alloca = llvm::dyn_cast<llvm::AllocaInst>(&x);
    if (not alloca)
        return failure("instruction is not alloca but: ", x.getOpcodeName());
    if (auto const result = std::forward<F_type>(f_type)(*alloca->getAllocatedType()); not result)
        return failure("alloca type predicate failed: ", result.message());
    if (auto const result = std::forward<F_size>(f_size)(*alloca->getArraySize()); not result)
        return failure("alloca array size predicate failed: ", result.message());
    if (auto const result = std::forward<F_align>(f_align)(alloca->getAlign()); not result)
        return failure("alloca alignment predicate failed: ", result.message());
    return true;
}

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_size, Predicate<llvm::Align> F_align>
boost::test_tools::predicate_result
    is_alloca(llvm::Instruction const* const p, F_type&& f_type, F_size&& f_size, F_align&& f_align)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_alloca(*p, std::forward<F_type>(f_type), std::forward<F_size>(f_size), std::forward<F_align>(f_align));
}

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_size, Predicate<llvm::Align> F_align>
boost::test_tools::predicate_result
    is_alloca(llvm::Value const& x, F_type&& f_type, F_size&& f_size, F_align&& f_align)
{
    auto const i = llvm::dyn_cast<llvm::Instruction>(&x);
    if (not i)
        return dep0::testing::failure("value is not an instruction but: ", to_string(x));
    return is_alloca(*i, std::forward<F_type>(f_type), std::forward<F_size>(f_size), std::forward<F_align>(f_align));
}

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_size, Predicate<llvm::Align> F_align>
boost::test_tools::predicate_result
    is_alloca(llvm::Value const* const p, F_type&& f_type, F_size&& f_size, F_align&& f_align)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_alloca(*p, std::forward<F_type>(f_type), std::forward<F_size>(f_size), std::forward<F_align>(f_align));
}

} // namespace impl

inline constexpr auto is_alloca = boost::hana::overload(
    [] <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_size, Predicate<llvm::Align> F_align>
    (llvm::Instruction const& x, F_type&& f_type, F_size&& f_size, F_align&& f_align)
    {
        return impl::is_alloca(
            x, std::forward<F_type>(f_type), std::forward<F_size>(f_size), std::forward<F_align>(f_align));
    },
    [] <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_size, Predicate<llvm::Align> F_align>
    (llvm::Instruction const* const p, F_type&& f_type, F_size&& f_size, F_align&& f_align)
    {
        return impl::is_alloca(
            p, std::forward<F_type>(f_type), std::forward<F_size>(f_size), std::forward<F_align>(f_align));
    },
    [] <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_size, Predicate<llvm::Align> F_align>
    (llvm::Value const& x, F_type&& f_type, F_size&& f_size, F_align&& f_align)
    {
        return impl::is_alloca(
            x, std::forward<F_type>(f_type), std::forward<F_size>(f_size), std::forward<F_align>(f_align));
    },
    [] <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_size, Predicate<llvm::Align> F_align>
    (llvm::Value const* const p, F_type&& f_type, F_size&& f_size, F_align&& f_align)
    {
        return impl::is_alloca(
            p, std::forward<F_type>(f_type), std::forward<F_size>(f_size), std::forward<F_align>(f_align));
    }
);

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_size, Predicate<llvm::Align> F_align>
auto alloca_of(F_type&& f_type, F_size&& f_size, F_align&& f_align)
{
    struct predicate_t
    {
        std::remove_cvref_t<F_type> f_type;
        std::remove_cvref_t<F_size> f_size;
        std::remove_cvref_t<F_align> f_align;

        boost::test_tools::predicate_result operator()(llvm::Instruction const& x) const
        {
            return impl::is_alloca(x, f_type, f_size, f_align);
        }

        boost::test_tools::predicate_result operator()(llvm::Value const& x) const
        {
            return impl::is_alloca(x, f_type, f_size, f_align);
        }
    };
    return predicate_t{std::forward<F_type>(f_type), std::forward<F_size>(f_size), std::forward<F_align>(f_align)};
}

} // namespace dep0::llvmgen::testing
