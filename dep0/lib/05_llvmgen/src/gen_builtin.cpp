#include "private/gen_builtin.hpp"

#include "private/gen_array.hpp"
#include "private/gen_type.hpp"
#include "private/gen_val.hpp"

namespace dep0::llvmgen {

/** Generate the LLVM value that results from a call to `::slice()`. */
static llvm::Value* gen_builtin_slice(
    global_ctx_t& global,
    local_ctx_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t::app_t const& app,
    llvm::Value* const dest)
{
    auto const& array = app.args[2];
    auto const properties = get_array_properties(std::get<typecheck::expr_t>(array.properties.sort.get()));
    auto const stride_size = gen_stride_size_if_needed(global, local, builder, properties);
    auto const type = gen_type(global, properties.element_type);
    auto const base = gen_val(global, local, builder, array, nullptr);
    auto const index = gen_val(global, local, builder, app.args[3], nullptr);
    auto const offset = stride_size ? builder.CreateMul(stride_size, index) : index;
    auto const ptr = builder.CreateGEP(type, base, offset);
    auto const& func_type = std::get<typecheck::expr_t>(app.func.get().properties.sort.get());
    auto const& pi_type = std::get<typecheck::expr_t::pi_t>(func_type.value);
    auto const& ret_type = pi_type.ret_type.get();
    return maybe_gen_store(global, local, builder, ptr, dest, ret_type);
}

llvm::Value* try_gen_builtin(
    global_ctx_t& global,
    local_ctx_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t::app_t const& app,
    llvm::Value* const dest)
{
    if (auto const g = std::get_if<typecheck::expr_t::global_t>(&app.func.get().value))
        if (g->module_name and g->module_name->empty() and g->name == "slice")
            return gen_builtin_slice(global, local, builder, app, dest);
    return nullptr;
}

} // namespace dep0::llvmgen

