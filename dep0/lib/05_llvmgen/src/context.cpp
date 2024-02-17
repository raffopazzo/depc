#include "private/context.hpp"

namespace dep0::llvmgen {

global_context_t::global_context_t(llvm::Module& m) :
    llvm_ctx(m.getContext()),
    llvm_module(m)
{ }

std::size_t global_context_t::get_next_id() { return next_id++; }

global_context_t::value_t* global_context_t::operator[](typecheck::expr_t::global_t const& k)
{
    return values[k];
}

global_context_t::value_t const* global_context_t::operator[](typecheck::expr_t::global_t const& k) const
{
    return values[k];
}

local_context_t::local_context_t(scope_map<typecheck::expr_t::var_t, value_t> values) :
    values(std::move(values))
{ }

local_context_t local_context_t::extend() const
{
    return local_context_t(values.extend());
}

local_context_t::value_t* local_context_t::operator[](typecheck::expr_t::var_t const& k)
{
    return values[k];
}

local_context_t::value_t const* local_context_t::operator[](typecheck::expr_t::var_t const& k) const
{
    return values[k];
}

} // namespace dep0::llvmgen
