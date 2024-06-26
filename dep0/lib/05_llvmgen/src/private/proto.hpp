#pragma once

#include "dep0/typecheck/ast.hpp"

#include <optional>
#include <vector>

namespace dep0::llvmgen {

/**
 * Proof that some `pi_t` or `abs_t` was a 1st order function type,
 * suitable to generate LLVM functions.
 * During LLVM code generation only runtime arguments are relevant,
 * i.e. arguments with quantity greater than zero.
 * So objects of this type contain only the runtime arguments.
 */
class llvm_func_proto_t
{
    std::vector<typecheck::func_arg_t> m_runtime_args;
    typecheck::expr_t m_ret_type;

    llvm_func_proto_t(typecheck::expr_t::pi_t const&);
    llvm_func_proto_t(typecheck::expr_t::abs_t const&);
    llvm_func_proto_t(llvm_func_proto_t const&) = delete; // prevent accidental copies
    llvm_func_proto_t& operator=(llvm_func_proto_t const&) = delete;

public:
    llvm_func_proto_t(llvm_func_proto_t&&) = default;
    llvm_func_proto_t& operator=(llvm_func_proto_t&&) = default;

    /**
     * Constructs a proof from a `pi_t`, if it is a 1st order function type.
     *
     * @return A valid LLVM function prototype, or nothing if the given function type is of higher order.
     */
    static std::optional<llvm_func_proto_t> from_pi(typecheck::expr_t::pi_t const&);


    /**
     * Constructs a proof from an `abs_t`, if it is a 1st order function abstraction.
     *
     * @return A valid LLVM function prototype, or nothing if the given function abstraction is of higher order.
     */
    static std::optional<llvm_func_proto_t> from_abs(typecheck::expr_t::abs_t const&);

    /**
     * @return A view of the runtime arguments of this function, all of which have a 1st order type.
     */
    std::vector<typecheck::func_arg_t> const& runtime_args() const { return m_runtime_args; }

    /**
     * @return The runtime argument at the given position.
     */
    typecheck::func_arg_t const& runtime_arg(std::size_t const i) const { return m_runtime_args[i]; }

    /**
     * @return The return type of the function prototype.
     */
    typecheck::expr_t const& ret_type() const { return m_ret_type; }
};

} // namespace dep0::llvmgen
