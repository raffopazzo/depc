#pragma once

#include <llvm/IR/Value.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

namespace impl {
boost::test_tools::predicate_result is_zeroinitializer(llvm::Value const&);
boost::test_tools::predicate_result is_zeroinitializer(llvm::Value const*);

} // namespace impl

inline constexpr auto is_zeroinitializer = boost::hana::overload(
    [] (llvm::Value const& x) { return impl::is_zeroinitializer(x); },
    [] (llvm::Value const* const p) { return impl::is_zeroinitializer(p); }
);

} // namespace dep0::llvmgen::testing
