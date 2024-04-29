#include "private/context.hpp"

namespace dep0::llvmgen {

global_ctx_t::global_ctx_t(llvm::Module& m) :
    llvm_ctx(m.getContext()),
    llvm_module(m)
{ }

std::size_t global_ctx_t::get_next_id() { return next_id++; }

global_ctx_t::value_t* global_ctx_t::operator[](typecheck::expr_t::global_t const& k)
{
    return values[k];
}

global_ctx_t::value_t const* global_ctx_t::operator[](typecheck::expr_t::global_t const& k) const
{
    return values[k];
}

local_ctx_t::local_ctx_t(scope_map<typecheck::expr_t::var_t, value_t> values) :
    values(std::move(values))
{ }

local_ctx_t local_ctx_t::extend() const
{
    return local_ctx_t(values.extend());
}

local_ctx_t::value_t* local_ctx_t::operator[](typecheck::expr_t::var_t const& k)
{
    return values[k];
}

local_ctx_t::value_t const* local_ctx_t::operator[](typecheck::expr_t::var_t const& k) const
{
    return values[k];
}

} // namespace dep0::llvmgen
