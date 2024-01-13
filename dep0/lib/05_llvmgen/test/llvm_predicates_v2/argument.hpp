#pragma once

#include "llvm_predicates_v2/predicate.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Argument.h>
#include <llvm/IR/Function.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

#include <optional>
#include <vector>

namespace dep0::llvmgen::testing::v2 {

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

} // namespace dep0::llvmgen::testing
