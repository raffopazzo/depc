#include "private/proto.hpp"

#include "private/first_order_types.hpp"

#include <algorithm>
#include <iterator>

namespace dep0::llvmgen {

static std::vector<typecheck::func_arg_t> extract_runtime_args(std::vector<typecheck::func_arg_t> const& args)
{
    std::vector<typecheck::func_arg_t> result;
    result.reserve(args.size());
    auto constexpr is_runtime_arg = [] (typecheck::func_arg_t const& arg) { return arg.qty > ast::qty_t::zero; };
    std::ranges::copy_if(args, std::back_inserter(result), is_runtime_arg);
    return result;
}

llvm_func_proto_t::llvm_func_proto_t(typecheck::expr_t::pi_t const& x) :
    m_runtime_args(extract_runtime_args(x.args)),
    m_ret_type(x.ret_type.get())
{ }

llvm_func_proto_t::llvm_func_proto_t(typecheck::expr_t::abs_t const& x) :
    m_runtime_args(extract_runtime_args(x.args)),
    m_ret_type(x.ret_type.get())
{ }

std::optional<llvm_func_proto_t> llvm_func_proto_t::from_pi(typecheck::expr_t::pi_t const& x)
{
    return is_first_order_function_type(x.args, x.ret_type.get()) ? std::optional{llvm_func_proto_t(x)} : std::nullopt;
}

std::optional<llvm_func_proto_t> llvm_func_proto_t::from_abs(typecheck::expr_t::abs_t const& x)
{
    return is_first_order_function_type(x.args, x.ret_type.get()) ? std::optional{llvm_func_proto_t(x)} : std::nullopt;
}

} // namespace dep0::llvmgen
