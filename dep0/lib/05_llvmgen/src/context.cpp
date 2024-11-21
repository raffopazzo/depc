/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
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

llvm::Value* global_ctx_t::get_string_literal(std::string_view const s)
{
    auto const it = string_literals.find(s);
    return it == string_literals.end() ? nullptr : it->second;
}

void global_ctx_t::store_string_literal(std::string s, llvm::Value* const v)
{
    string_literals.emplace(std::move(s), v);
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
