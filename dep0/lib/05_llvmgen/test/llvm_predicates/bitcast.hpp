#pragma once

#include "llvm_predicates/predicate.hpp"
#include "llvm_predicates/to_string.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Instructions.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

namespace impl {

template <Predicate<llvm::Value> F_op, Predicate<llvm::Type> F_src, Predicate<llvm::Type> F_dst>
boost::test_tools::predicate_result
    is_bitcast_of(llvm::Instruction const& x, F_op&& f_op, F_src&& f_src, F_dst&& f_dst)
{
    using namespace dep0::testing;
    auto const bitcast = llvm::dyn_cast<llvm::BitCastInst>(&x);
    if (not bitcast)
        return failure("instruction is not a bitcast but: ", to_string(x));
    if (auto const result = std::forward<F_op>(f_op)(*bitcast->getOperand(0ul)); not result)
        return failure("bitcast operand predicate failed: ", result.message());
    if (auto const result = std::forward<F_src>(f_src)(*bitcast->getSrcTy()); not result)
        return failure("bitcast source type predicate failed: ", result.message());
    if (auto const result = std::forward<F_dst>(f_dst)(*bitcast->getDestTy()); not result)
        return failure("bitcast dstination type predicate failed: ", result.message());
    return boost::test_tools::predicate_result(true);
}

template <Predicate<llvm::Value> F_op, Predicate<llvm::Type> F_src, Predicate<llvm::Type> F_dst>
boost::test_tools::predicate_result
    is_bitcast_of(llvm::Instruction const* const p, F_op&& f_op, F_src&& f_src, F_dst&& f_dst)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_bitcast_of(*p, std::forward<F_op>(f_op), std::forward<F_src>(f_src), std::forward<F_dst>(f_dst));
}

template <Predicate<llvm::Value> F_op, Predicate<llvm::Type> F_src, Predicate<llvm::Type> F_dst>
boost::test_tools::predicate_result
    is_bitcast_of(llvm::Value const& x, F_op&& f_op, F_src&& f_src, F_dst&& f_dst)
{
    auto const i = llvm::dyn_cast<llvm::Instruction>(&x);
    if (not i)
        return dep0::testing::failure("value is not an instruction but: ", to_string(x));
    return is_bitcast_of(*i, std::forward<F_op>(f_op), std::forward<F_src>(f_src), std::forward<F_dst>(f_dst));
}

template <Predicate<llvm::Value> F_op, Predicate<llvm::Type> F_src, Predicate<llvm::Type> F_dst>
boost::test_tools::predicate_result
    is_bitcast_of(llvm::Value const* const p, F_op&& f_op, F_src&& f_src, F_dst&& f_dst)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_bitcast_of(*p, std::forward<F_op>(f_op), std::forward<F_src>(f_src), std::forward<F_dst>(f_dst));
}

} // namespace impl

inline constexpr auto is_bitcast_of =  boost::hana::overload(
    [] <Predicate<llvm::Value> F_op, Predicate<llvm::Type> F_src, Predicate<llvm::Type> F_dst>
    (llvm::Instruction const& x, F_op&& f_op, F_src&& f_src, F_dst&& f_dst)
    {
        return impl::is_bitcast_of(x, std::forward<F_op>(f_op), std::forward<F_src>(f_src), std::forward<F_dst>(f_dst));
    },
    [] <Predicate<llvm::Value> F_op, Predicate<llvm::Type> F_src, Predicate<llvm::Type> F_dst>
    (llvm::Instruction const* const p, F_op&& f_op, F_src&& f_src, F_dst&& f_dst)
    {
        return impl::is_bitcast_of(p, std::forward<F_op>(f_op), std::forward<F_src>(f_src), std::forward<F_dst>(f_dst));
    },
    [] <Predicate<llvm::Value> F_op, Predicate<llvm::Type> F_src, Predicate<llvm::Type> F_dst>
    (llvm::Value const& x, F_op&& f_op, F_src&& f_src, F_dst&& f_dst)
    {
        return impl::is_bitcast_of(x, std::forward<F_op>(f_op), std::forward<F_src>(f_src), std::forward<F_dst>(f_dst));
    },
    [] <Predicate<llvm::Value> F_op, Predicate<llvm::Type> F_src, Predicate<llvm::Type> F_dst>
    (llvm::Value const* const p, F_op&& f_op, F_src&& f_src, F_dst&& f_dst)
    {
        return impl::is_bitcast_of(p, std::forward<F_op>(f_op), std::forward<F_src>(f_src), std::forward<F_dst>(f_dst));
    }
);

template <Predicate<llvm::Value> F_op, Predicate<llvm::Type> F_src, Predicate<llvm::Type> F_dst>
auto bitcast_of(F_op&& f_op, F_src&& f_src, F_dst&& f_dst)
{
    struct predicate_t
    {
        std::remove_cvref_t<F_op> f_op;
        std::remove_cvref_t<F_src> f_src;
        std::remove_cvref_t<F_dst> f_dst;

        boost::test_tools::predicate_result operator()(llvm::Instruction const& x) const
        {
            return is_bitcast_of(x, f_op, f_src, f_dst);
        }

        boost::test_tools::predicate_result operator()(llvm::Value const& x) const
        {
            return is_bitcast_of(x, f_op, f_src, f_dst);
        }
    };
    return predicate_t{std::forward<F_op>(f_op), std::forward<F_src>(f_src), std::forward<F_dst>(f_dst)};
}

} // namespace dep0::llvmgen::testing

