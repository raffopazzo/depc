#pragma once

#include "llvm_predicates_v2/predicate.hpp"
#include "llvm_predicates_v2/to_string.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Instructions.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing::v2 {

namespace impl {

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Align> F_align>
boost::test_tools::predicate_result
    is_load_of(llvm::Instruction const& x, F_type&& f_type, F_ptr&& f_ptr, F_align&& f_align)
{
    using namespace dep0::testing;
    auto const load = llvm::dyn_cast<llvm::LoadInst>(&x);
    if (not load)
        return failure("instruction is not load but: ", x.getOpcodeName());
    if (auto const result = std::forward<F_type>(f_type)(*load->getType()); not result)
        return failure("load type predicate failed: ", result.message());
    if (auto const result = std::forward<F_ptr>(f_ptr)(*load->getPointerOperand()); not result)
        return failure("load pointer operand predicate failed: ", result.message());
    if (auto const result = std::forward<F_align>(f_align)(load->getAlign()); not result)
        return failure("load align predicate failed: ", result.message());
    return true;
}

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Align> F_align>
boost::test_tools::predicate_result
    is_load_of(llvm::Instruction const* const p, F_type&& f_type, F_ptr&& f_ptr, F_align&& f_align)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_load_of(*p, std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_align>(f_align));
}

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Align> F_align>
boost::test_tools::predicate_result
    is_load_of(llvm::Value const& x, F_type&& f_type, F_ptr&& f_ptr, F_align&& f_align)
{
    auto const inst = llvm::dyn_cast<llvm::Instruction>(&x);
    if (not inst)
        return dep0::testing::failure("value is not an instruction but: ", to_string(x));
    return is_load_of(*inst, std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_align>(f_align));
}

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Align> F_align>
boost::test_tools::predicate_result
    is_load_of(llvm::Value const* const p, F_type&& f_type, F_ptr&& f_ptr, F_align&& f_align)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_load_of(*p, std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_align>(f_align));
}

} // namespace impl

inline constexpr auto is_load_of = boost::hana::overload(
    [] <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Align> F_align>
    (llvm::Instruction const& x, F_type&& f_type, F_ptr&& f_ptr, F_align&& f_align)
    {
        return impl::is_load_of(
            x, std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_align>(f_align));
    },
    [] <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Align> F_align>
    (llvm::Instruction const* const p, F_type&& f_type, F_ptr&& f_ptr, F_align&& f_align)
    {
        return impl::is_load_of(
            p, std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_align>(f_align));
    },
    [] <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Align> F_align>
    (llvm::Value const& x, F_type&& f_type, F_ptr&& f_ptr, F_align&& f_align)
    {
        return impl::is_load_of(
            x, std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_align>(f_align));
    },
    [] <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Align> F_align>
    (llvm::Value const* const p, F_type&& f_type, F_ptr&& f_ptr, F_align&& f_align)
    {
        return impl::is_load_of(
            p, std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_align>(f_align));
    }
);

template <Predicate<llvm::Type> F_type, Predicate<llvm::Value> F_ptr, Predicate<llvm::Align> F_align>
auto load_of(F_type&& f_type, F_ptr&& f_ptr, F_align&& f_align)
{
    struct predicate_t
    {
        std::remove_cvref_t<F_type> f_type;
        std::remove_cvref_t<F_ptr> f_ptr;
        std::remove_cvref_t<F_align> f_align;

        boost::test_tools::predicate_result operator()(llvm::Instruction const& x) const
        {
            return impl::is_load_of(x, f_type, f_ptr, f_align);
        }

        boost::test_tools::predicate_result operator()(llvm::Value const& x) const
        {
            return impl::is_load_of(x, f_type, f_ptr, f_align);
        }
    };
    return predicate_t{std::forward<F_type>(f_type), std::forward<F_ptr>(f_ptr), std::forward<F_align>(f_align)};
}

} // namespace dep0::llvmgen::testing

