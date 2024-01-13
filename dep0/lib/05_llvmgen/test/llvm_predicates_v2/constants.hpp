#pragma once

#include <llvm/IR/Value.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing::v2 {

namespace impl {

boost::test_tools::predicate_result is_constant(llvm::Value const&, int);
boost::test_tools::predicate_result is_constant(llvm::Value const*, int);

boost::test_tools::predicate_result is_constant(llvm::Value const&, std::size_t);
boost::test_tools::predicate_result is_constant(llvm::Value const*, std::size_t);

} // namespace impl

inline constexpr auto is_constant = boost::hana::overload(
    [] (llvm::Value const& x, int const v) { return impl::is_constant(x, v); },
    [] (llvm::Value const* const p, int const v) { return impl::is_constant(p, v); },
    [] (llvm::Value const& x, std::size_t const v) { return impl::is_constant(x, v); },
    [] (llvm::Value const* const p, std::size_t const v) { return impl::is_constant(p, v); }
);

inline auto constant(int const v)
{
    return [v] (llvm::Value const& x)
    {
        return impl::is_constant(x, v);
    };
}

inline auto constant(std::size_t const v)
{
    return [v] (llvm::Value const& x)
    {
        return impl::is_constant(x, v);
    };
}

} // namespace dep0::llvmgen::testing
