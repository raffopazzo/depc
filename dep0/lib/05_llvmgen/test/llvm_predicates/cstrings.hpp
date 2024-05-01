#pragma once

#include <llvm/IR/Value.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

namespace impl {

boost::test_tools::predicate_result is_cstr(llvm::Value const&, std::string_view);
boost::test_tools::predicate_result is_cstr(llvm::Value const*, std::string_view);

} // namespace impl

inline constexpr auto is_cstr = boost::hana::overload(
    [] (llvm::Value const& x, std::string_view const v) { return impl::is_cstr(x, v); },
    [] (llvm::Value const* const p, std::string_view const v) { return impl::is_cstr(p, v); }
);

inline auto cstr(std::string v)
{
    return [v=std::move(v)] (llvm::Value const& x)
    {
        return impl::is_cstr(x, v);
    };
}

} // namespace dep0::llvmgen::testing
