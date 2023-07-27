#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/ast/concepts.hpp"

#include <optional>
#include <ostream>

namespace dep0::typecheck {

struct legal_module_t;
struct legal_type_def_t;
struct legal_func_def_t;
struct legal_func_call_t;
struct legal_type_t;
struct legal_body_t;
struct legal_stmt_t;
struct legal_expr_t;

struct properties_t
{
    using module_properties_type = legal_module_t;
    using type_def_properties_type = legal_type_def_t;
    using func_def_properties_type = legal_func_def_t;
    using func_call_properties_type = legal_func_call_t;
    using type_properties_type = legal_type_t;
    using body_properties_type = legal_body_t;
    using stmt_properties_type = legal_stmt_t;
    using expr_properties_type = legal_expr_t;
};
static_assert(ast::Properties<properties_t>);

using module_t = ast::module_t<properties_t>;
using type_def_t = ast::type_def_t<properties_t>;
using func_def_t = ast::func_def_t<properties_t>;
using func_call_t = ast::func_call_t<properties_t>;
using type_t = ast::type_t<properties_t>;
using body_t = ast::body_t<properties_t>;
using stmt_t = ast::stmt_t<properties_t>;
using expr_t = ast::expr_t<properties_t>;

template <typename>
struct derivation_t // TODO move to derivation.hpp
{
    derivation_t(derivation_t const&) = default;
    derivation_t& operator=(derivation_t const&) = default;
    derivation_t(derivation_t&&) = default;
    derivation_t& operator=(derivation_t&&) = default;

    bool operator==(derivation_t const&) const = default;

private:
    friend struct derivation_rules;
    derivation_t() = default;
};

struct legal_module_t
{
    // here I would like to express that a module is legal if *all* its functions, types, etc are legal;
    // but this requires dependent types... which is why I'm writing DepC in the first place
    derivation_t<module_t> derivation;

    bool operator==(legal_module_t const&) const = default;
};

struct legal_type_def_t
{
    // TODO should there be a derivation here? from the formation rule I guess?
    derivation_t<type_def_t> derivation;
    bool operator==(legal_type_def_t const&) const = default;
};

struct legal_type_t
{
    derivation_t<type_t> derivation;
    bool operator==(legal_type_t const&) const = default;
};

struct legal_func_def_t
{
    derivation_t<func_def_t> derivation;
    bool operator==(legal_func_def_t const&) const = default;
};

struct legal_func_call_t
{
    derivation_t<func_call_t> derivation;
    type_t ret_type;
    bool operator==(legal_func_call_t const&) const = default;
};

struct legal_body_t
{
    derivation_t<body_t> derivation;
    bool operator==(legal_body_t const&) const = default;
};

struct legal_stmt_t
{
    derivation_t<stmt_t> derivation;
    bool operator==(legal_stmt_t const&) const = default;
};

struct legal_expr_t
{
    derivation_t<expr_t> derivation;
    type_t type;
    bool operator==(legal_expr_t const&) const = default;
};

std::ostream& pretty_print(std::ostream&, type_t const&);
std::ostream& pretty_print(std::ostream&, expr_t const&);

} // namespace dep0::typecheck
