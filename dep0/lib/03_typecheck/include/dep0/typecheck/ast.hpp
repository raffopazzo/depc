#pragma once

#include "dep0/typecheck/tt/derivation.hpp"

#include "dep0/ast/ast.hpp"
#include "dep0/ast/concepts.hpp"

#include <optional>

namespace dep0::typecheck {

struct legal_module_t;
struct legal_func_def_t;
struct legal_type_t;
struct legal_body_t;
struct legal_stmt_t;
struct legal_expr_t;

struct properties_t
{
    using module_properties_type = legal_module_t;
    using func_def_properties_type = legal_func_def_t;
    using type_properties_type = legal_type_t;
    using body_properties_type = legal_body_t;
    using stmt_properties_type = legal_stmt_t;
    using expr_properties_type = legal_expr_t;
};
static_assert(ast::Properties<properties_t>);

using module_t = ast::module_t<properties_t>;
using func_def_t = ast::func_def_t<properties_t>;
using type_t = ast::type_t<properties_t>;
using body_t = ast::body_t<properties_t>;
using stmt_t = ast::stmt_t<properties_t>;
using expr_t = ast::expr_t<properties_t>;

struct legal_module_t
{
    // here I would like to express that a module is legal if *all* its functions, types, etc are legal;
    // but this requires dependent types... which is why I'm writing DepC in the first place
};

struct legal_func_def_t
{
};

struct legal_type_t
{
    tt::derivation_t derivation;
};

struct legal_body_t
{
};

struct legal_stmt_t
{
};

struct legal_expr_t
{
    tt::derivation_t derivation;
    type_t type;
};

} // namespace dep0::typecheck
