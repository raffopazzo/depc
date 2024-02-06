#pragma once

#include "llvm_predicates/predicate.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Instructions.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

namespace impl {

template <Predicate<llvm::Type> F_ret, Predicate<llvm::Argument>... Args>
boost::test_tools::predicate_result
    is_function_of(
        llvm::Function const& x,
        std::tuple<Args...> args,
        F_ret&& f,
        std::vector<llvm::Attribute::AttrKind> const& attributes = {})
{
    using namespace dep0::testing;
    auto constexpr N = sizeof...(Args);
    if (x.arg_size() != N)
        return failure("wrong number of arguments: ", N, " != ", x.arg_size());
    auto const ret_type = x.getReturnType();
    if (not ret_type)
        return failure("function has no return type");
    if (auto const result = std::forward<F_ret>(f)(*ret_type); not result)
        return failure("predicate failed for return type: ", result.message());
    auto const& ret_attrs = x.getAttributes().getRetAttributes();
    if (ret_attrs.getNumAttributes() != attributes.size())
        return failure("wrong number of return attributes: ", ret_attrs.getNumAttributes(), " != " ,attributes.size());
    for (auto const& attr: attributes)
        if (not ret_attrs.hasAttribute(attr))
            return failure("return attribute not set: ", llvm::Attribute::getNameFromAttrKind(attr).str());
    auto result = boost::test_tools::predicate_result(true);
    [&] <std::size_t... Is> (std::index_sequence<Is...>)
    {
        ([&]
        {
            if (result)
            {
                if (auto const arg = x.getArg(Is); not arg)
                    result = failure("function argument ", Is, " is null");
                else if (auto const tmp = std::get<Is>(args)(*arg); not tmp)
                    result = failure("argument predicate at index ", Is, " failed: ", tmp.message());
            }
        }(), ...);
    } (std::make_index_sequence<N>{});
    return result;
}

template <Predicate<llvm::Type> F_ret, Predicate<llvm::Argument>... Args>
boost::test_tools::predicate_result
    is_function_of(
        llvm::Function const* const p,
        std::tuple<Args...> args,
        F_ret&& f,
        std::vector<llvm::Attribute::AttrKind> const& attributes = {})
{
    if (not p)
        return dep0::testing::failure("function is null");
    return is_function_of(*p, std::move(args), std::forward<F_ret>(f), attributes);
}

} // namespace impl

inline constexpr auto is_function_of = boost::hana::overload(
    [] <Predicate<llvm::Type> F_ret, Predicate<llvm::Argument>... Args>
    (
        llvm::Function const& x,
        std::tuple<Args...> args,
        F_ret&& f,
        std::vector<llvm::Attribute::AttrKind> const& attributes = {}
    )
    {
        return impl::is_function_of(x, std::move(args), std::forward<F_ret>(f), attributes);
    },
    [] <Predicate<llvm::Type> F_ret, Predicate<llvm::Argument>... Args>
    (
        llvm::Function const* const p,
        std::tuple<Args...> args,
        F_ret&& f,
        std::vector<llvm::Attribute::AttrKind> const& attributes = {}
    )
    {
        return impl::is_function_of(p, std::move(args), std::forward<F_ret>(f), attributes);
    }
);

} // namespace dep0::llvmgen::testing
