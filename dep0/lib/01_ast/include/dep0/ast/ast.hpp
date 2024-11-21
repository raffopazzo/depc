/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/ast/attribute.hpp"
#include "dep0/ast/concepts.hpp"
#include "dep0/ast/qty.hpp"
#include "dep0/ast/sign.hpp"
#include "dep0/ast/mutable.hpp"
#include "dep0/ast/width.hpp"

#include "dep0/source.hpp"

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/variant/recursive_wrapper.hpp>

#include <optional>
#include <tuple>
#include <vector>
#include <variant>

namespace dep0::ast {

// forward declarations

template <Properties P> struct module_t;
template <Properties P> struct type_def_t;
template <Properties P> struct axiom_t;
template <Properties P> struct extern_decl_t;
template <Properties P> struct func_decl_t;
template <Properties P> struct func_def_t;
template <Properties P> struct func_arg_t;
template <Properties P> struct body_t;
template <Properties P> struct stmt_t;
template <Properties P> struct expr_t;

// definitions

/** Represents a sequence of one or more statements, for example the body of a function or of an if-else branch. */
template <Properties P>
struct body_t
{
    using properties_t = typename P::body_properties_type;
    using stmt_t = ast::stmt_t<P>;

    properties_t properties;
    std::vector<stmt_t> stmts;
};

/**
 * Represents an expression from the Lambda Calculus of Construction with Definitions,
 * and with extensions specific to DepC.
 *
 * See `docs/01_type_theory.md` for more details on Type Theory.
 */
template <Properties P>
struct expr_t
{
    using rec_t = boost::recursive_wrapper<expr_t>;
    using properties_t = typename P::expr_properties_type;

    /** Represents the `typename` keyword, whose values are types; for example `i32_t` and `bool`. */
    struct typename_t {};

    /** Represents the type constructor `true_t`, whose type is `(bool) -> typename`. */
    struct true_t {};

    /**
     * Represents the keyword 'auto' when used inside an expression.
     * It is a placeholder for a value that must be automatically filled by the compiler.
     */
    struct auto_t {};

    /** Represents the primitive type `bool`, whose values are `true` or `false`. */
    struct bool_t {};

    /** Represents the primitive type `cstr_t`, whose values are string literals. */
    struct cstr_t {};

    /**
     * Represents the primitive type `unit_t`.
     *
     * It only has a single value and can always be constructed "from nowhere", aka "the top type".
     * It is similar to `void` in C/C++ but it can also be used inside structs and arrays.
     */
    struct unit_t {};

    /** Represents the primitive type `i8_t`, with values from the range `[-128, +127]`. */
    struct i8_t {};

    /** Represents the primitive type `i16_t`, with values from the range `[-32768, +32767]`. */
    struct i16_t {};

    /** Represents the primitive type `i16_t`, with values from the range `[-2147483648, +2147483647]`. */
    struct i32_t {};

    /**
     * Represents the primitive type `i32_t`, with values from the range `[-9223372036854775808, +9223372036854775807]`
     */
    struct i64_t {};

    /** Represents the primitive type `u8_t`, with values from the range `[0, 255]`. */
    struct u8_t {};

    /** Represents the primitive type `u16_t`, with values from the range `[0, 65535]`. */
    struct u16_t {};

    /** Represents the primitive type `u32_t`, with values from the range `[0, 4294967295]`. */
    struct u32_t {};

    /** Represents the primitive type `u64_t`, with values from the range `[0, 18446744073709551615]`. */
    struct u64_t {};

    /** Represents the boolean constants `true` or `false`. */
    struct boolean_constant_t
    {
        bool value;
    };

    /**
     * Represents numeric constants, for example `-99` or `18446744073709551615`.
     *
     * Note that differently from boolean constants, numeric constants cannot be typechecked "on their own".
     * For example, the constant `0` can be assigned to all primitive and user-defined integral types.
     *
     * One might argue that it is possible to assign a type to a numerical constant if,
     * in the current context, there is only 1 viable type in which the numerical constant would fit,
     * eg max of u64, provided the user did not define their own 64 bits integer;
     * but having this kind of context-sensitivity is also a bit suprising, so not really great;
     * therefore the decision is to always fail type-assignment of numerical constants.
     */
    struct numeric_constant_t
    {
        boost::multiprecision::cpp_int value;
    };

    /** Represents string literals, like "" and "Hello \"World\"". */
    struct string_literal_t
    {
        source_text value;
    };

    /** Represents a boolean expression, for example `x and not y or is_even(k)`. */
    struct boolean_expr_t
    {
        struct not_t { rec_t expr; };
        struct and_t { rec_t lhs, rhs; };
        struct or_t  { rec_t lhs, rhs; };
        using value_t = std::variant<not_t, and_t, or_t>;
        value_t value;
    };

    /** Represents a relational expression, for example `x == y`, `x <= y` or `x > f(y)`. */
    struct relation_expr_t
    {
        struct eq_t  { rec_t lhs, rhs; };
        struct neq_t { rec_t lhs, rhs; };
        struct gt_t  { rec_t lhs, rhs; };
        struct gte_t { rec_t lhs, rhs; };
        struct lt_t  { rec_t lhs, rhs; };
        struct lte_t { rec_t lhs, rhs; };
        using value_t = std::variant<eq_t, neq_t, gt_t, gte_t, lt_t, lte_t>;
        value_t value;
    };

    /** Represents an arithmetic expression, for example `x + y`. */
    struct arith_expr_t
    {
        struct plus_t { rec_t lhs, rhs; };
        struct minus_t { rec_t lhs, rhs; };
        struct mult_t { rec_t lhs, rhs; };
        struct div_t { rec_t lhs, rhs; };
        using value_t = std::variant<plus_t, minus_t, mult_t, div_t>;
        value_t value;
    };

    /**
     * Represents the name of a local variable in the current context, for example `x`.
     *
     * The field `idx` starts from 0 for all user-defined variables.
     * It might be increased by alpha-conversion when performing capture-avoiding substitution.
     *
     * Note that during the parsing stage, all identifiers are mapped to a `var_t`,
     * including those which are really referring to global names.
     * Later, during type-checking, a `var_t` can be "upgraded" to a `global_t` depending
     * on whether the look-up of the name was resolved from the global environment or the local context.
     */
    struct var_t
    {
        source_text name;
        std::size_t idx = 0ul;
        bool operator<(var_t const& that) const { return std::tie(name, idx) < std::tie(that.name, that.idx); }
        bool operator==(var_t const&) const = default;
    };

    /**
     * Represents a (possibly qualified) name of a global symbol, defined either:
     *   - in the current module, for example `f` or `int` (in which case it is unqualified);
     *   - in some imported module, for example `mylib::f` or `mylib::int`;
     *   - in the prelude module, for example `::basic_axiom`.
     * Note that only the 1st one is an unqualified identifier;
     * the other two are qualified (and the prelude module is referred to by the empty string).
     *
     * Therefore `module_name` can be:
     *   - an empty optional, for globals defined in the current module;
     *   - an empty string, for globals defined in the prelude module;
     *   - the name of the imported module that defines the global.
     *
     * @remarks
     *      Without knowledge of the current environment and context,
     *      an unqualified identifier (say `f') could refer to either:
     *        - a global symbol from the current module
     *        - or a local variable of the current function.
     *      The parser does not currently track this, so it will always emit a `var_t` for an unqualified identifier;
     *      during type-checking, if `f` refers to a global, the `var_t` is "upgraded" to a `global_t`.
     */
    struct global_t
    {
        std::optional<source_text> module_name;
        source_text name;
        bool operator<(global_t const& that) const
        {
            return std::tie(module_name, name) < std::tie(that.module_name, that.name);
        }
        bool operator==(global_t const&) const = default;
    };

    /**
     * Represents an uncarried function application, for example `f(x, y, z)` or `select(which)(x, y, z)`.
     *
     * In this example the expressions `f` and `select(which)` are stored in `func`,
     * and `args` contains all arguments `x, y, z`.
     */
    struct app_t
    {
        rec_t func;
        std::vector<expr_t> args;
    };

    /**
     * Represents an uncarried function abstraction,
     * either as a lambda or embedded inside a global function definition.
     */
    struct abs_t
    {
        is_mutable_t is_mutable;
        std::vector<func_arg_t<P>> args;
        rec_t ret_type;
        body_t<P> body;
    };

    /**
     * Represents an uncarried Pi-type, either for lambdas or global function definitions.
     *
     * A Pi-type can be as simple as a function type, for example `(i32_t, i32_t) -> i32_t`,
     * or a dependent type, for example `(typename t, u64_t n) -> array_t(t, n)`.
     */
    struct pi_t
    {
        is_mutable_t is_mutable;
        std::vector<func_arg_t<P>> args;
        rec_t ret_type;
    };

    /**
     * Represents the keyword `array_t`, which on its own has type `(typename, u64_t) -> typename`.
     *
     * A "traditional" array is therefore the application (i.e. an `app_t`) of `array_t` to 2 arguments:
     * the first is the element type and second is the size of the array.
     * Multidimensional arrays are arrays whose element type is another array.
     */
    struct array_t {};

    /**
     * Represents an initializer list, for example `{1,2,3}` or `{{1,2},{3,4},{5,6}}` for multidimensional arrays.
     *
     * Currently, initializer lists can only be used to construct arrays,
     * but in future they can be used to initialize structs too.
     */
    struct init_list_t
    {
        std::vector<expr_t> values;
    };

    /** Represents an array member access (aka subscript operator), for example `xs[1]`, where `xs` is an array. */
    struct subscript_t
    {
        rec_t array;
        rec_t index;
    };

    /**
     * Represents an expression of the form `value because reason`,
     * where `reason` is some auxillary proof that the compiler can use
     * to verify that `value` is legal.
     *
     * @remarks
     *      Because of "proof irrelevance", two because-expressions whose values
     *      are alpha-equivalent are treated as alpha-equivalent, regardless of the two reasons.
     *      For example `xs[i] because proof1` and `xs[i] because proof2`
     *      are considered alpha-equivalent, since both prove that `i` is a valid index
     *      and the exact reason why the index is valid is irrelevant.
     *      A similar argument applies even if one of the expressions is not `because`;
     *      for example, in a context where `xs` is an array of 2 elements,
     *      both `xs[0]` and `xs[0] because true_t(0 < 2)` are alpha-equivalent,
     *      because whatever proof the compiler used in the first expression is just as
     *      good as the auxillary proof explicitly supplied in the second one.
     */
    struct because_t
    {
        rec_t value;
        rec_t reason;
    };

    using value_t =
        std::variant<
            typename_t, true_t, auto_t,
            bool_t, cstr_t, unit_t, i8_t, i16_t, i32_t, i64_t, u8_t, u16_t, u32_t, u64_t,
            boolean_constant_t, numeric_constant_t, string_literal_t,
            boolean_expr_t, relation_expr_t, arith_expr_t,
            var_t, global_t, app_t, abs_t, pi_t,
            array_t, init_list_t, subscript_t, because_t
        >;

    properties_t properties;
    value_t value;
};

/**
 * If the argument is `app_t` whose function term is `array_t`, returns the `app_t` node; `nullptr` otherwise.
 *
 * This is useful to determine whether a type is an array, for example `array_t(i32_t, n)`.
 * If this function is called on a typechecked type expression,
 * the vector of arguments of the `app_t` node is guaranteed to have exactly 2 elements:
 * the first is the array element type and the second is the size of the array.
 * Multidimensional arrays are arrays whose element type is another array.
 */
template <Properties P>
typename expr_t<P>::app_t const* get_if_app_of_array(expr_t<P> const& x)
{
    auto const app = std::get_if<typename expr_t<P>::app_t>(&x.value);
    return app and std::holds_alternative<typename expr_t<P>::array_t>(app->func.get().value) ? app : nullptr;
}

/**
 * Represents a function argument, i.e. its type and (optional) name.
 *
 * This can be used inside:
 *   - function definitions, for example `i32_t f(i32_t, i32_t x) { ... }`
 *   - function types, for example `(i32_t, i32_t x) -> i32_t`
 */
template <Properties P>
struct func_arg_t
{
    using properties_t = typename P::func_arg_properties_type;
    using expr_t = ast::expr_t<P>;

    properties_t properties;
    qty_t qty;
    expr_t type;
    std::optional<typename expr_t::var_t> var;
};

/**
 * Represents a statement inside a body,
 * for example a function call (presumably with side effects),
 * an `if-else` statement, a `return` statement, etc.
 */
template <Properties P>
struct stmt_t
{
    using properties_t = typename P::stmt_properties_type;
    using body_t = ast::body_t<P>;
    using expr_t = ast::expr_t<P>;

    /** Represents an `if` or `if-else` statement, whose condition must be of type `bool`. */
    struct if_else_t
    {
        expr_t cond;
        body_t true_branch;
        std::optional<body_t> false_branch;
    };

    /**
     * Represents a `return` statement, which can be empty only inside functions returning `unit_t`,
     * otherwise a value of the expected return type must be supplied.
     * @remarks Because of dependent types, the return type of a function may be different in different branches.
     */
    struct return_t
    {
        std::optional<expr_t> expr;
    };

    /**
     * The `impossible` statement marks as unreachable the current branch of execution.
     * A proof of false, i.e. a value of type `true_t(false)`, is required to type-check this statement.
     * The compiler is allowed to remove all code leading to an `impossible` statement.
     * For example, if `impossible` is inside the `false` branch of an `if` statement,
     * this branch can be removed, possibly removing the condition too.
     * An alternative form `impossible because expr` can also be used to explicitly provide a proof of false.
     */
    struct impossible_t
    {
        std::optional<expr_t> reason;
    };

    using value_t = std::variant<typename expr_t::app_t, if_else_t, return_t, impossible_t>;

    properties_t properties;
    value_t value;
};

/**
 * Represents a user-defined integral type definition.
 *
 * At the moment only integral types can be defined,
 * but it's possible to imagine extending this for structs too.
 */
template <Properties P>
struct type_def_t
{
    struct integer_t
    {
        source_text name;
        sign_t sign;
        width_t width;
    };

    using properties_t = typename P::type_def_properties_type;
    using value_t = std::variant<integer_t>;
    properties_t properties;
    value_t value;
};

/**
 * Axioms are like function declarations, except they are not followed by a function definition.
 * In the Curry-Howard isomorphism, they are true propositions that cannot be proved.
 * Introducing the wrong set of axioms may lead to an inconsistent theory.
 */
template <Properties P>
struct axiom_t
{
    using properties_t = typename P::axiom_properties_type;

    properties_t properties;
    source_text name;
    expr_t<P>::pi_t signature;
};

/**
 * Represents an extern function declaration.
 * It is exactly like a function declaration but for functions provided by some external library
 * and written in a different language but with a C interface.
 * They can only be invoked from functions marked as `mutable`.
 */
template <Properties P>
struct extern_decl_t
{
    using properties_t = typename P::extern_decl_properties_type;

    properties_t properties;
    source_text name;
    expr_t<P>::pi_t signature;
};

/**
 * Represents a global function declaration,
 * which is comprised of a name and a Pi-type for its signature and return type.
 */
template <Properties P>
struct func_decl_t
{
    using properties_t = typename P::func_decl_properties_type;

    properties_t properties;
    source_text name;
    std::optional<attribute_t> attribute;
    expr_t<P>::pi_t signature;
};

/**
 * Represents a global function definition,
 * which is comprised of a name and a "lambda abstraction" within it.
 */
template <Properties P>
struct func_def_t
{
    using properties_t = typename P::func_def_properties_type;

    properties_t properties;
    source_text name;
    std::optional<attribute_t> attribute;
    expr_t<P>::abs_t value;
};

/**
 * Represents an entire module of DepC code,
 * made of type definitions, function definitions, function declarations, etc.
 *
 * In future this might be extended to include imported modules.
 */
template <Properties P>
struct module_t
{
    using properties_t = typename P::module_properties_type;
    using type_def_t = ast::type_def_t<P>;
    using axiom_t = ast::axiom_t<P>;
    using extern_decl_t = ast::extern_decl_t<P>;
    using func_decl_t = ast::func_decl_t<P>;
    using func_def_t = ast::func_def_t<P>;
    using entry_t = std::variant<type_def_t, axiom_t, extern_decl_t, func_decl_t, func_def_t>;

    properties_t properties;
    std::vector<entry_t> entries;
};

} // namespace dep0::ast
