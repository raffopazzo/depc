#pragma once

#include "dep0/typecheck/derivation.hpp"

#include "dep0/ast/ast.hpp"
#include "dep0/ast/concepts.hpp"

#include "dep0/source.hpp"

#include <boost/variant/recursive_wrapper.hpp>

#include <optional>
#include <ostream>

namespace dep0::typecheck {

struct legal_module_t;
struct legal_type_def_t;
struct legal_axiom_t;
struct legal_extern_decl_t;
struct legal_func_decl_t;
struct legal_func_def_t;
struct legal_func_arg_t;
struct legal_body_t;
struct legal_stmt_t;
struct legal_expr_t;

struct properties_t
{
    using module_properties_type = legal_module_t;
    using type_def_properties_type = legal_type_def_t;
    using axiom_properties_type = legal_axiom_t;
    using extern_decl_properties_type = legal_extern_decl_t;
    using func_decl_properties_type = legal_func_decl_t;
    using func_def_properties_type = legal_func_def_t;
    using func_arg_properties_type = legal_func_arg_t;
    using body_properties_type = legal_body_t;
    using stmt_properties_type = legal_stmt_t;
    using expr_properties_type = legal_expr_t;
};
static_assert(ast::Properties<properties_t>);

using module_t = ast::module_t<properties_t>;
using type_def_t = ast::type_def_t<properties_t>;
using axiom_t = ast::axiom_t<properties_t>;
using extern_decl_t = ast::extern_decl_t<properties_t>;
using func_decl_t = ast::func_decl_t<properties_t>;
using func_def_t = ast::func_def_t<properties_t>;
using func_arg_t = ast::func_arg_t<properties_t>;
using body_t = ast::body_t<properties_t>;
using stmt_t = ast::stmt_t<properties_t>;
using expr_t = ast::expr_t<properties_t>;

struct kind_t{};
using sort_t = std::variant<expr_t, kind_t>;

struct legal_module_t
{
    // here I would like to express that a module is legal if *all* its functions, types, etc are legal;
    // but this requires dependent types... which is why I'm writing DepC in the first place
    derivation_t<module_t> derivation;

    bool operator==(legal_module_t const&) const = default;
};

struct legal_type_def_t
{
    source_loc_t origin;
    derivation_t<type_def_t> derivation;
    bool operator==(legal_type_def_t const&) const = default;
};

struct legal_axiom_t
{
    source_loc_t origin;
    derivation_t<axiom_t> derivation;
    boost::recursive_wrapper<sort_t> sort;
    bool operator==(legal_axiom_t const&) const = default;
};

struct legal_extern_decl_t
{
    source_loc_t origin;
    derivation_t<extern_decl_t> derivation;
    boost::recursive_wrapper<sort_t> sort;
    bool operator==(legal_extern_decl_t const&) const = default;
};

struct legal_func_decl_t
{
    source_loc_t origin;
    derivation_t<func_decl_t> derivation;
    boost::recursive_wrapper<sort_t> sort;
    bool operator==(legal_func_decl_t const&) const = default;
};

struct legal_func_def_t
{
    source_loc_t origin;
    derivation_t<func_def_t> derivation;
    boost::recursive_wrapper<sort_t> sort;
    bool operator==(legal_func_def_t const&) const = default;
};

struct legal_func_arg_t
{
    derivation_t<func_arg_t> derivation;
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
    boost::recursive_wrapper<sort_t> sort;
    bool operator==(legal_expr_t const&) const = default;
};

std::ostream& pretty_print(std::ostream&, kind_t, std::size_t indent = 0ul);
std::ostream& pretty_print(std::ostream&, sort_t const&, std::size_t indent = 0ul);

} // namespace dep0::typecheck
