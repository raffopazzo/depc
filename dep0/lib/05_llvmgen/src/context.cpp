/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/context.hpp"

#include "private/gen_array.hpp"

#include "dep0/ast/alpha_equivalence.hpp"

namespace dep0::llvmgen {

// TODO duplicated from proof_search.cpp; should we extract something in common?
bool global_ctx_t::eq_t::operator()(typecheck::expr_t const& x, typecheck::expr_t const& y) const
{
    // TODO should this be beta-delta equivalence instead? needs a test
    return ast::is_alpha_equivalent(x, y).has_value()
        and std::visit(
            boost::hana::overload(
                [] (typecheck::expr_t const& x_type, typecheck::expr_t const& y_type)
                {
                    return ast::is_alpha_equivalent(x_type, y_type).has_value();
                },
                [] (typecheck::kind_t, typecheck::kind_t)
                {
                    return true;
                },
                [] <typename T, typename U> requires (not std::is_same_v<T, U>)
                (T const&, U const&)
                {
                    return false;
                }),
            x.properties.sort.get(), y.properties.sort.get());
}

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

std::optional<llvm_func_t> global_ctx_t::get_destructor(typecheck::expr_t const& type) const
{
    std::optional<llvm_func_t> result;
    if (auto const because = std::get_if<typecheck::expr_t::because_t>(&type.value))
        result = get_destructor(because->value.get());
    else if (auto const properties = get_properties_if_array(type))
    {
        auto const it = array_destructors.find(properties->element_type);
        if (it != array_destructors.end())
            result.emplace(it->second);
    }
    else
    {
        auto const it = non_array_destructors.find(type);
        if (it != non_array_destructors.end())
            result.emplace(it->second);
    }
    return result;
}

void global_ctx_t::store_destructor(typecheck::expr_t type, llvm_func_t func)
{
    if (auto const because = std::get_if<typecheck::expr_t::because_t>(&type.value))
        store_destructor(std::move(because->value.get()), func);
    else if (auto const properties = get_properties_if_array(type))
        array_destructors.emplace(properties->element_type, std::move(func));
    else
        non_array_destructors.emplace(std::move(type), std::move(func));
}

llvm::Value* global_ctx_t::get_string_literal(std::string_view const s) const
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
