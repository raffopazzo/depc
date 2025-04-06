/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

namespace impl {

boost::test_tools::predicate_result is_memcpy(llvm::Function const&);
boost::test_tools::predicate_result is_memcpy(llvm::Function const*);
boost::test_tools::predicate_result is_memcpy(llvm::Value const&);
boost::test_tools::predicate_result is_memcpy(llvm::Value const*);

} // namespace impl

inline constexpr auto is_memcpy = boost::hana::overload(
    [] (llvm::Instruction const& x) { return impl::is_memcpy(x); },
    [] (llvm::Instruction const* const p) { return impl::is_memcpy(p); },
    [] (llvm::Value const& x) { return impl::is_memcpy(x); },
    [] (llvm::Value const* const p) { return impl::is_memcpy(p); }
);

} // namespace dep0::llvmgen::testing

