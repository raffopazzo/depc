#include "private/proto.hpp"

#include "private/first_order_types.hpp"

namespace dep0::llvmgen {

llvm_func_proto_t::llvm_func_proto_t(typecheck::expr_t::pi_t const& x) :
    m_args(x.args),
    m_ret_type(x.ret_type.get())
{ }

llvm_func_proto_t::llvm_func_proto_t(typecheck::expr_t::abs_t const& x) :
    m_args(x.args),
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
