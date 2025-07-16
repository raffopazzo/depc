/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Specializes the generic AST for the typechecking stage.
 * @see @ref dep0_design_ast
 */
#pragma once

#include "dep0/typecheck/derivation.hpp"

#include "dep0/ast/ast.hpp"
#include "dep0/ast/concepts.hpp"

#include "dep0/source.hpp"

#include <boost/variant/recursive_wrapper.hpp>

#include <optional>
#include <ostream>

namespace dep0::typecheck {

/** @cond DEP0_DOXYGEN_HIDE */
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
/** @endcond */

/**
 * @brief Trait types required to specialize the AST for the parser stage.
 *
 * After typechecking succeeds we have obtained a "legal" AST.
 * An expression is "legal" if a type can be assigned to it.
 * In particular, the `properties` field of an `expr_t` contains the type assigned to the expression.
 *
 * @remarks Currently the `derivation` field of any AST node does not really prove anything.
 * It only proves that the node was constructed via `derivation_rules` but, ideally,
 * it would really contain a proof that the content of the node is legal.
 * This, however, requires dependent types, hence DepC...
 */
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

/**
 * @brief All types "have type" *Kind*, for example `%i32_t` can be assigned to variables "of type" `typename`.
 * @see @ref kinds
 */
struct kind_t{};

/**
 * @brief An expression has either type *Kind* or a "real" type.
 *
 * For example, `%i32_t` has type *Kind* but `0` has type `%i32_t` which is itself an expression.
 * Similarly, arrays have type `%array_t(%i32_t, n)` which is also an expression:
 * in particular the result of the application of the *Type Constructor* `%array_t`.
 *
 * @see @ref kinds
 */
using sort_t = std::variant<expr_t, kind_t>;

struct env_t; // environment.hpp depends on expr_t; so we forward declare env_t here to break the cycle
struct legal_module_t
{
    derivation_t<module_t> derivation;
    boost::recursive_wrapper<env_t> env;

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

/** @brief Overload of `dep0::ast::pretty_print()` for `kind_t`. */
std::ostream& pretty_print(std::ostream&, kind_t, std::size_t indent = 0ul);

/** @brief Overload of `dep0::ast::pretty_print()` for `sort_t`. */
std::ostream& pretty_print(std::ostream&, sort_t const&, std::size_t indent = 0ul);

} // namespace dep0::typecheck

// it is not sufficient to only forward declare `env_t`,
// we also need to include it here in order to make its destructor visible,
// otherwise `boost::recursive_wrapper` does not compile
#include "dep0/typecheck/environment.hpp"
