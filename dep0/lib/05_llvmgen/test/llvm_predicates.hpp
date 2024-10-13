#pragma once

#include "llvm_predicates/add.hpp"
#include "llvm_predicates/align.hpp"
#include "llvm_predicates/alloca.hpp"
#include "llvm_predicates/and.hpp"
#include "llvm_predicates/argument.hpp"
#include "llvm_predicates/bitcast.hpp"
#include "llvm_predicates/branch.hpp"
#include "llvm_predicates/call.hpp"
#include "llvm_predicates/cmp.hpp"
#include "llvm_predicates/constants.hpp"
#include "llvm_predicates/cstrings.hpp"
#include "llvm_predicates/function.hpp"
#include "llvm_predicates/function_type.hpp"
#include "llvm_predicates/gep.hpp"
#include "llvm_predicates/global.hpp"
#include "llvm_predicates/load.hpp"
#include "llvm_predicates/mul.hpp"
#include "llvm_predicates/or.hpp"
#include "llvm_predicates/return.hpp"
#include "llvm_predicates/select.hpp"
#include "llvm_predicates/store.hpp"
#include "llvm_predicates/sub.hpp"
#include "llvm_predicates/types.hpp"
#include "llvm_predicates/unreachable.hpp"
#include "llvm_predicates/xor.hpp"
#include "llvm_predicates/zeroinitializer.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Value.h>

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

inline auto exactly(llvm::Value const* const p)
{
    return [p] (llvm::Value const& x) -> boost::test_tools::predicate_result
    {
        if (p != &x)
            return dep0::testing::failure(p, " != ", &x);
        else
            return true;
    };
}

} // namespace dep0::llvmgen::testing
