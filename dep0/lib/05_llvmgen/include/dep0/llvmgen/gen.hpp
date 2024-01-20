#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"
#include "dep0/unique_ref.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <memory>
#include <string_view>

namespace dep0::llvmgen {

enum class verify_t { yes, no };

expected<unique_ref<llvm::Module>> gen(
    llvm::LLVMContext&,
    std::string_view module_name,
    typecheck::module_t const& m,
    verify_t = verify_t::yes
) noexcept;

}
