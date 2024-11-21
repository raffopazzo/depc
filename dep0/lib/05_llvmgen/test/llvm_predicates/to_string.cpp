/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "llvm_predicates/to_string.hpp"

namespace dep0::llvmgen::testing {

std::string to_string(llvm::Type const& x)
{
    std::string result;
    llvm::raw_string_ostream s(result);
    x.print(s);
    return result;
}

std::string to_string(llvm::Value const& x)
{
    std::string result;
    llvm::raw_string_ostream s(result);
    x.print(s);
    return result;
}

} // namespace dep0::llvmgen::testing
