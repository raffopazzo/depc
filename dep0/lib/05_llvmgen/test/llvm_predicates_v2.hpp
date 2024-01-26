#pragma once

#include "llvm_predicates_v2/add.hpp"
#include "llvm_predicates_v2/align.hpp"
#include "llvm_predicates_v2/alloca.hpp"
#include "llvm_predicates_v2/argument.hpp"
#include "llvm_predicates_v2/bitcast.hpp"
#include "llvm_predicates_v2/branch.hpp"
#include "llvm_predicates_v2/call.hpp"
#include "llvm_predicates_v2/constants.hpp"
#include "llvm_predicates_v2/function.hpp"
#include "llvm_predicates_v2/function_type.hpp"
#include "llvm_predicates_v2/gep.hpp"
#include "llvm_predicates_v2/load.hpp"
#include "llvm_predicates_v2/mul.hpp"
#include "llvm_predicates_v2/return.hpp"
#include "llvm_predicates_v2/select.hpp"
#include "llvm_predicates_v2/store.hpp"
#include "llvm_predicates_v2/types.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Value.h>

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing::v2 {

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

} // namespace dep0::llvmgen::testing::v2
