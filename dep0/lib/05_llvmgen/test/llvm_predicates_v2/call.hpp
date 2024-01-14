#pragma once

#include "llvm_predicates_v2/predicate.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Instructions.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing::v2 {

template <Predicate<llvm::Value> F>
struct call_arg_t
{
    F predicate;
    std::vector<llvm::Attribute::AttrKind> attributes;
};

template <Predicate<llvm::Value> F>
call_arg_t<std::remove_cvref_t<F>> call_arg(F&& f, std::vector<llvm::Attribute::AttrKind> attributes = {})
{
    return {std::forward<F>(f), std::move(attributes)};
}

namespace impl {

template <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
boost::test_tools::predicate_result is_direct_call(llvm::Instruction const& x, F&& f, call_arg_t<Args>... args)
{
    using namespace dep0::testing;
    auto const call = llvm::dyn_cast<llvm::CallInst>(&x);
    if (not call)
        return failure("instruction is not a call but: ", x.getOpcodeName());
    if (call->isIndirectCall())
        return failure("call is indirect but should be direct");
    auto constexpr N = sizeof...(Args);
    if (auto const n_arg = call->getNumArgOperands(); n_arg != N)
        return failure("wrong number of call arguments: ", N, " != ", n_arg);
    auto result = boost::test_tools::predicate_result(true);
    int next = 0;
    ([&]
    {
        if (result)
        {
            auto const i = next++;
            auto const arg = call->getArgOperand(i);
            if (not arg)
            {
                result = failure("call argument ", i, " is null");
                return;
            }
            if (auto const tmp = args.predicate(*arg); not tmp)
            {
                result = failure("call argument predicate at index ", i, " failed: ", tmp.message());
                return;
            }
            auto const& actual_attributes = call->getAttributes().getParamAttributes(i);
            if (args.attributes.size() != actual_attributes.getNumAttributes())
            {
                result = failure(
                    "wrong number of call argument attributes at index ", i, ": ",
                    args.attributes.size(), " != ", actual_attributes.getNumAttributes());
                return;
            }
            for (auto const& attr: args.attributes)
                if (not actual_attributes.hasAttribute(attr))
                {
                    result = failure(
                        "call argument attribute not set at index ", i, ": ",
                        llvm::Attribute::getNameFromAttrKind(attr).str());
                    return;
                }
        }
    }(), ...);
    return result;
}

template <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
boost::test_tools::predicate_result is_direct_call(llvm::Instruction const* const p, F&& f, call_arg_t<Args>... args)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_direct_call(*p, std::forward<F>(f), std::move(args)...);
}

template <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
boost::test_tools::predicate_result is_direct_call(llvm::Value const& x, F&& f, call_arg_t<Args>... args)
{
    auto const i = llvm::dyn_cast<llvm::Instruction>(&x);
    if (not i)
        return dep0::testing::failure("value is not an instruction but: ValueID=", x.getValueID());
    return is_direct_call(*i, std::forward<F>(f), std::move(args)...);
}

template <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
boost::test_tools::predicate_result is_direct_call(llvm::Value const* const p, F&& f, call_arg_t<Args>... args)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_direct_call(*p, std::forward<F>(f), std::move(args)...);
}

} // namespace impl

inline constexpr auto is_direct_call = boost::hana::overload(
    [] <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
    (llvm::Instruction const& x, F&& f, call_arg_t<Args>... args)
    {
        return impl::is_direct_call(x, std::forward<F>(f), std::move(args)...);
    },
    [] <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
    (llvm::Instruction const* const p, F&& f, call_arg_t<Args>... args)
    {
        return impl::is_direct_call(p, std::forward<F>(f), std::move(args)...);
    },
    [] <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
    (llvm::Value const& x, F&& f, call_arg_t<Args>... args)
    {
        return impl::is_direct_call(x, std::forward<F>(f), std::move(args)...);
    },
    [] <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
    (llvm::Value const* const p, F&& f, call_arg_t<Args>... args)
    {
        return impl::is_direct_call(p, std::forward<F>(f), std::move(args)...);
    }
);

template <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
auto direct_call_of(F&& f, call_arg_t<Args>... args)
{
    struct predicate_t
    {
        std::remove_cvref_t<F> f;
        std::tuple<call_arg_t<Args>...> args;

        boost::test_tools::predicate_result operator()(llvm::Instruction const& x) const
        {
            return std::apply(
                is_direct_call,
                [&] <std::size_t... Is> (std::index_sequence<Is...>)
                {
                    return std::forward_as_tuple(x, f, std::get<Is>(args)...);
                }(std::make_index_sequence<sizeof...(Args)>{}));
        }

        boost::test_tools::predicate_result operator()(llvm::Value const& x) const
        {
            return std::apply(
                is_direct_call,
                [&] <std::size_t... Is> (std::index_sequence<Is...>)
                {
                    return std::forward_as_tuple(x, f, std::get<Is>(args)...);
                }(std::make_index_sequence<sizeof...(Args)>{}));
        }
    };
    return predicate_t{std::forward<F>(f), std::move(args)...};
}

} // namespace dep0::llvmgen::testing
