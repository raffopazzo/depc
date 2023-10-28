#pragma once

#include "dep0/typecheck/derivation.hpp"

#include "dep0/ast/ast.hpp"
#include "dep0/ast/concepts.hpp"

#include <optional>
#include <ostream>

namespace dep0::typecheck {

struct legal_module_t;
struct legal_type_def_t;
struct legal_func_def_t;
struct legal_func_arg_t;
struct legal_type_t;
struct legal_body_t;
struct legal_stmt_t;
struct legal_expr_t;

struct properties_t
{
    using module_properties_type = legal_module_t;
    using type_def_properties_type = legal_type_def_t;
    using func_def_properties_type = legal_func_def_t;
    using func_arg_properties_type = legal_func_arg_t;
    using type_properties_type = legal_type_t;
    using body_properties_type = legal_body_t;
    using stmt_properties_type = legal_stmt_t;
    using expr_properties_type = legal_expr_t;
};
static_assert(ast::Properties<properties_t>);

using module_t = ast::module_t<properties_t>;
using type_def_t = ast::type_def_t<properties_t>;
using func_def_t = ast::func_def_t<properties_t>;
using func_arg_t = ast::func_arg_t<properties_t>;
using type_t = ast::type_t<properties_t>;
using body_t = ast::body_t<properties_t>;
using stmt_t = ast::stmt_t<properties_t>;
using expr_t = ast::expr_t<properties_t>;
using sort_t = ast::sort_t<properties_t>;

struct legal_module_t
{
    // here I would like to express that a module is legal if *all* its functions, types, etc are legal;
    // but this requires dependent types... which is why I'm writing DepC in the first place
    derivation_t<module_t> derivation;

    bool operator==(legal_module_t const&) const = default;
};

struct legal_type_def_t
{
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

struct legal_func_arg_t
{
    derivation_t<func_arg_t> derivation;
    // could consider adding a field `sort_t sort` but it's redundant as the same information is in func_arg_t
    bool operator==(legal_func_arg_t const&) const = default;
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
    sort_t sort;
    bool operator==(legal_expr_t const&) const = default;
};

} // namespace dep0::typecheck
