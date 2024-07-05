#pragma once
#include "llvm_predicates/predicate.hpp"
#include "llvm_predicates/to_string.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/GlobalVariable.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

namespace impl {

template <Predicate<llvm::Value> F>
boost::test_tools::predicate_result is_global_of(llvm::Value const& x, F&& f)
{
    using namespace dep0::testing;
    auto const v = dyn_cast<llvm::GlobalVariable>(&x);
    if (not v)
        return failure("value is not a global value but: ", to_string(x));
    if (not v->hasInitializer())
        return failure("global value has no initializer");
    if (auto const result = std::forward<F>(f)(*v->getInitializer()); not result)
        return failure("inside global initializer: ", result.message());
    return true;
}

template <Predicate<llvm::Value> F>
boost::test_tools::predicate_result is_global_of(llvm::Value const* const p, F&& f)
{
    using namespace dep0::testing;
    if (not p)
        return failure("value is null");
    return is_global_of(*p, std::forward<F>(f));
}

} // namespace impl

inline constexpr auto is_global_of = boost::hana::overload(
    [] <Predicate<llvm::Value> F>
    (llvm::Value const& x, F&& f)
    {
        return impl::is_global_of(x, std::forward<F>(f));
    },
    [] <Predicate<llvm::Value> F>
    (llvm::Value const* const p, F&& f)
    {
        return impl::is_global_of(p, std::forward<F>(f));
    }
);

template <Predicate<llvm::Value> F>
auto global_of(F&& f)
{
    return [f=std::forward<F>(f)] (llvm::Value const& x)
    {
        return impl::is_global_of(x, f);
    };
}

} // namespace dep0::llvmgen::testing
