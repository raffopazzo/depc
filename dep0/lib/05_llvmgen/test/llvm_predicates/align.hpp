/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include <llvm/Support/Alignment.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

namespace impl {

boost::test_tools::predicate_result is_align_of(llvm::Align const&, std::size_t);
boost::test_tools::predicate_result is_align_of(llvm::Align const*, std::size_t);

} // namespace impl

inline constexpr auto is_align_of = boost::hana::overload(
    [] (llvm::Align const& x, std::size_t const value) { return impl::is_align_of(x, value); },
    [] (llvm::Align const* const p, std::size_t const value) { return impl::is_align_of(p, value); }
);

inline auto align_of(std::size_t const value)
{
    return [value] (llvm::Align const& x)
    {
        return impl::is_align_of(x, value);
    };
}

} // namespace dep0::llvmgen::testing

