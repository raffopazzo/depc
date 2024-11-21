/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace dep0::llvmgen::testing {

std::string to_string(llvm::Type const&);
std::string to_string(llvm::Value const&);

} // namespace dep0::llvmgen::testing
