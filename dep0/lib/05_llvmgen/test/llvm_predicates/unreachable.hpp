/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "llvm_predicates/predicate.hpp"

#include <llvm/IR/Instructions.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

namespace impl {

boost::test_tools::predicate_result is_unreachable(llvm::Instruction const&);
boost::test_tools::predicate_result is_unreachable(llvm::Instruction const*);

} // namespace impl

inline constexpr auto is_unreachable = boost::hana::overload(
    [] (llvm::Instruction const& x) { return impl::is_unreachable(x); },
    [] (llvm::Instruction const* const p) { return impl::is_unreachable(p); });

} // namespace dep0::llvmgen::testing
