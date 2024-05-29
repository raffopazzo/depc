#pragma once

#include "llvm_predicates/predicate.hpp"
#include "llvm_predicates/to_string.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Instructions.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

template <Predicate<llvm::Value> F>
struct call_arg_t
{
    F predicate;
    std::vector<llvm::Attribute::AttrKind> attributes;
    llvm::MaybeAlign alignment;
};

template <Predicate<llvm::Value> F>
call_arg_t<std::remove_cvref_t<F>>
    call_arg(
        F&& f,
        std::vector<llvm::Attribute::AttrKind> attributes = {},
        llvm::MaybeAlign alignment = {})
{
    return {std::forward<F>(f), std::move(attributes), std::move(alignment)};
}

namespace impl {

enum class call_style_t { direct, indirect };

template <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
boost::test_tools::predicate_result
    is_call(call_style_t const style, llvm::Instruction const& x, F&& f, call_arg_t<Args>... args)
{
    using namespace dep0::testing;
    auto const call = llvm::dyn_cast<llvm::CallInst>(&x);
    if (not call)
        return failure("instruction is not a call but: ", to_string(x));
    auto constexpr N = sizeof...(Args);
    if (auto const n_arg = call->getNumArgOperands(); n_arg != N)
        return failure("wrong number of call arguments: ", N, " != ", n_arg);
    if (style == call_style_t::indirect)
    {
        if (not call->isIndirectCall())
            return failure("call should be indirect but is direct");
    }
    else
        if (call->isIndirectCall())
            return failure("call is indirect but should be direct");
    if (auto const result = std::forward<F>(f)(*call->getCalledOperand()); not result)
        return failure("inside invoked function: ", result.message());
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
                result = failure("inside call argument ", i, ": ", tmp.message());
                return;
            }
            auto const& actual_attrs = call->getAttributes().getParamAttributes(i);
            if (args.attributes.size() != actual_attrs.getNumAttributes())
            {
                result = failure(
                    "call argument ", i, " has wrong number of attributes: ",
                    args.attributes.size(), " != ", actual_attrs.getNumAttributes());
                return;
            }
            for (auto const& attr: args.attributes)
            {
                if (not actual_attrs.hasAttribute(attr))
                {
                    result = failure(
                        "call argument ", i, " does not have attribute ",
                        llvm::Attribute::getNameFromAttrKind(attr).str());
                    return;
                }
                if (auto const& align = actual_attrs.getAlignment(); align.hasValue() xor args.alignment.hasValue())
                {
                    if (args.alignment)
                        result = failure("call argument ", i, " does not have alignment ");
                    else
                        result = failure("call argument ", i, " has alignment but it should not");
                    return;
                }
                else if (args.alignment and args.alignment.getValue() != align.getValue())
                {
                    result = failure(
                        "call argument ", i, " has wrong alignment: ",
                        args.alignment.getValue().value(), " != ", align.getValue().value());
                    return;
                }
            }
        }
    }(), ...);
    return result;
}

template <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
boost::test_tools::predicate_result
    is_call(call_style_t const style, llvm::Instruction const* const p, F&& f, call_arg_t<Args>... args)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_call(style, *p, std::forward<F>(f), std::move(args)...);
}

template <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
boost::test_tools::predicate_result
    is_call(call_style_t const style, llvm::Value const& x, F&& f, call_arg_t<Args>... args)
{
    auto const i = llvm::dyn_cast<llvm::Instruction>(&x);
    if (not i)
        return dep0::testing::failure("value is not an instruction but: ", to_string(x));
    return is_call(style, *i, std::forward<F>(f), std::move(args)...);
}

template <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
boost::test_tools::predicate_result
    is_call(call_style_t const style, llvm::Value const* const p, F&& f, call_arg_t<Args>... args)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_call(style, *p, std::forward<F>(f), std::move(args)...);
}

} // namespace impl

inline constexpr auto is_direct_call = boost::hana::overload(
    [] <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
    (llvm::Instruction const& x, F&& f, call_arg_t<Args>... args)
    {
        return impl::is_call(impl::call_style_t::direct, x, std::forward<F>(f), std::move(args)...);
    },
    [] <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
    (llvm::Instruction const* const p, F&& f, call_arg_t<Args>... args)
    {
        return impl::is_call(impl::call_style_t::direct, p, std::forward<F>(f), std::move(args)...);
    },
    [] <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
    (llvm::Value const& x, F&& f, call_arg_t<Args>... args)
    {
        return impl::is_call(impl::call_style_t::direct, x, std::forward<F>(f), std::move(args)...);
    },
    [] <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
    (llvm::Value const* const p, F&& f, call_arg_t<Args>... args)
    {
        return impl::is_call(impl::call_style_t::direct, p, std::forward<F>(f), std::move(args)...);
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
    return predicate_t{std::forward<F>(f), {std::move(args)...}};
}

inline constexpr auto is_indirect_call = boost::hana::overload(
    [] <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
    (llvm::Instruction const& x, F&& f, call_arg_t<Args>... args)
    {
        return impl::is_call(impl::call_style_t::indirect, x, std::forward<F>(f), std::move(args)...);
    },
    [] <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
    (llvm::Instruction const* const p, F&& f, call_arg_t<Args>... args)
    {
        return impl::is_call(impl::call_style_t::indirect, p, std::forward<F>(f), std::move(args)...);
    },
    [] <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
    (llvm::Value const& x, F&& f, call_arg_t<Args>... args)
    {
        return impl::is_call(impl::call_style_t::indirect, x, std::forward<F>(f), std::move(args)...);
    },
    [] <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
    (llvm::Value const* const p, F&& f, call_arg_t<Args>... args)
    {
        return impl::is_call(impl::call_style_t::indirect, p, std::forward<F>(f), std::move(args)...);
    }
);

template <Predicate<llvm::Value> F, Predicate<llvm::Value>... Args>
auto indirect_call_of(F&& f, call_arg_t<Args>... args)
{
    struct predicate_t
    {
        std::remove_cvref_t<F> f;
        std::tuple<call_arg_t<Args>...> args;

        boost::test_tools::predicate_result operator()(llvm::Instruction const& x) const
        {
            return std::apply(
                is_indirect_call,
                [&] <std::size_t... Is> (std::index_sequence<Is...>)
                {
                    return std::forward_as_tuple(x, f, std::get<Is>(args)...);
                }(std::make_index_sequence<sizeof...(Args)>{}));
        }

        boost::test_tools::predicate_result operator()(llvm::Value const& x) const
        {
            return std::apply(
                is_indirect_call,
                [&] <std::size_t... Is> (std::index_sequence<Is...>)
                {
                    return std::forward_as_tuple(x, f, std::get<Is>(args)...);
                }(std::make_index_sequence<sizeof...(Args)>{}));
        }
    };
    return predicate_t{std::forward<F>(f), {std::move(args)...}};
}

} // namespace dep0::llvmgen::testing
