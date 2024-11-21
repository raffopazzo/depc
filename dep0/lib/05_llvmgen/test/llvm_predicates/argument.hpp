/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "llvm_predicates/predicate.hpp"
#include "llvm_predicates/types.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Argument.h>
#include <llvm/IR/Function.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

#include <optional>
#include <vector>

namespace dep0::llvmgen::testing {

namespace impl {

template <Predicate<llvm::Type> F_type>
boost::test_tools::predicate_result is_arg(
    llvm::Argument const& x,
    F_type&& f_type,
    std::optional<std::string_view> const name,
    std::vector<llvm::Attribute::AttrKind> const& attributes)
{
    using namespace dep0::testing;
    if (auto const arg_type = x.getType(); not arg_type)
        return failure("argument type is null");
    else if (auto const result = std::forward<F_type>(f_type)(*arg_type); not result)
        return failure("predicate failed for argument type: ", result.message());

    if (name)
    {
        if (auto const arg_name = static_cast<std::string_view>(x.getName()); *name != arg_name)
            return failure("argument name [", arg_name, "] != [", *name, ']');
    }
    else
    {
        if (not x.getName().empty())
            return failure("argument should be anonymous but has name: ", x.getName().str());
    }

    if (auto const n_attr = x.getParent()->getAttributes().getParamAttributes(x.getArgNo()).getNumAttributes();
        attributes.size() != n_attr)
    {
        return failure("wrong number of argument attributes: ", attributes.size(), " != ", n_attr);
    }
    for (auto const& attr: attributes)
        if (not x.hasAttribute(attr))
            return failure("argument attribute not set: ", llvm::Attribute::getNameFromAttrKind(attr).str());

    return true;
}

template <Predicate<llvm::Type> F_type>
boost::test_tools::predicate_result is_arg(
    llvm::Argument const* const p,
    F_type&& f_type,
    std::optional<std::string_view> const name,
    std::vector<llvm::Attribute::AttrKind> const& attributes)
{
    if (not p)
        return dep0::testing::failure("argument is null");
    return is_arg(*p, std::forward<F_type>(f_type), name, std::move(attributes));
}

} // namespace impl

inline constexpr auto is_arg = boost::hana::overload(
    [] <Predicate<llvm::Type> F_type>
    (
        llvm::Argument const& x,
        F_type&& f_type,
        std::optional<std::string_view> const name = std::nullopt,
        std::vector<llvm::Attribute::AttrKind> const& attributes = {}
    )
    {
        return impl::is_arg(x, std::forward<F_type>(f_type), name, attributes);
    },
    [] <Predicate<llvm::Type> F_type>
    (
        llvm::Argument const* const p,
        F_type&& f_type,
        std::optional<std::string_view> const name = std::nullopt,
        std::vector<llvm::Attribute::AttrKind> const& attributes = {}
    )
    {
        return impl::is_arg(p, std::forward<F_type>(f_type), name, attributes);
    }
);

template <Predicate<llvm::Type> F_type>
auto arg_of(
    F_type&& f_type,
    std::optional<std::string_view> const name = std::nullopt,
    std::vector<llvm::Attribute::AttrKind> attributes = {})
{
    return [f_type=std::forward<F_type>(f_type), name, attributes=std::move(attributes)] (llvm::Argument const& x)
    {
        return impl::is_arg(x, f_type, name, attributes);
    };
}

template <Predicate<llvm::Type> F>
auto ret_ptr_to(F&& f)
{
    return arg_of(pointer_to(std::forward<F>(f)), std::nullopt, {llvm::Attribute::NonNull, llvm::Attribute::StructRet});
}

} // namespace dep0::llvmgen::testing
