#pragma once

#include "private/context.hpp"

#include "dep0/typecheck/ast.hpp"

#include <llvm/IR/Attributes.h>

namespace dep0::llvmgen {

/**
 * @return The sign extension attribute for the given type expression, or None if not an integral type.
 */
llvm::Attribute::AttrKind get_sign_ext_attribute(global_ctx_t const&, typecheck::expr_t const& type);

} // namespace dep0::llvmgen

