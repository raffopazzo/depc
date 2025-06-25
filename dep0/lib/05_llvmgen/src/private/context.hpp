/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Facilities to keep track of the global and local contexts during the LLVM codegen stage.
 */
#pragma once

#include "private/llvm_func.hpp"

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/environment.hpp"

#include "dep0/ast/hash_code.hpp"
#include "dep0/scope_map.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include <variant>

#ifndef NDEBUG
#include <cassert>
#endif

namespace dep0::llvmgen {

/**
 * @brief Holds together things that have global visibility during IR codegen.
 *
 * In particular:
 *   - the LLVM module being generated
 *   - the `llvm::LLVMContext` being used
 *   - global function declarations and type definitions
 *   - and other pieces of global state.
 */
struct global_ctx_t
{
    /** @brief Holds information about global typedefs together with their corresponding LLVM type. */
    struct type_def_t
    {
        typecheck::type_def_t def;
        llvm::Type* llvm_type; // nullptr if not a 1st order type
    };

    /** @brief Global functions and type definitions, whose key is a `typecheck::expr_t::global_t`. */
    using value_t = std::variant<llvm_func_t, type_def_t>;
    using iterator = scope_map<typecheck::expr_t::global_t, value_t>::iterator;

    llvm::LLVMContext& llvm_ctx;
    llvm::Module& llvm_module;
    typecheck::env_t const& env;

    /**
     * @brief Obtain a unique ID, useful for example to generate a unique name for anonymous function.
     * @remarks Uniqueness is guaranteed only for IDs generated form the same global context.
     */
    std::size_t get_next_id();

    /**
     * @brief Construct a new global context that will be used to generate LLVM IR for the given module.
     * @param env The environment used during type-checking of the module.
     * @warning You must use one global context per LLVM module.
     */
    global_ctx_t(typecheck::env_t const& env, llvm::Module&);

    global_ctx_t(global_ctx_t const&) = delete;
    global_ctx_t(global_ctx_t&&) = delete;
    global_ctx_t& operator=(global_ctx_t const&) = delete;
    global_ctx_t& operator=(global_ctx_t&&) = delete;

    value_t* operator[](typecheck::expr_t::global_t const&);
    value_t const* operator[](typecheck::expr_t::global_t const&) const;

    /** @brief Returns the destructor for the given type, if one exists; otherwise `nullopt`. */
    std::optional<llvm_func_t> get_destructor(typecheck::expr_t const& type) const;

    /** @brief Store a new destructor for the given type and overwrites the old one if one already exists. */
    void store_destructor(typecheck::expr_t type, llvm_func_t);

    /**
     * @brief Look up the LLVM global value stored for the given string literal.
     * @details Return `nullptr` if no global value exists yet.
     * @see @ref `store_string_literal()`.
     */
    llvm::Constant* get_string_literal(typecheck::expr_t::string_literal_t const&) const;

    /**
     * @brief Store an LLVM global value holding the given string literal
     * @remarks If one already exists, it will be overwritten.
     * @see @ref `get_string_literal()`.
     */
    void store_string_literal(typecheck::expr_t::string_literal_t const&, llvm::Constant*);

    /** @brief Retrieve the address previously saved for the given global, or `nullptr` if none saved yet. */
    llvm::Value* load_address(typecheck::expr_t::global_t const&) const;

    /** @brief Retrieve the address previously saved for the given string literal, or `nullptr` if none saved yet. */
    llvm::Value* load_address(typecheck::expr_t::string_literal_t const&) const;

    /** @brief Save the address where the given global is stored. */
    void save_address(typecheck::expr_t::global_t const&, llvm::Value* address);

    /** @brief Save the address where the given string literal is stored. */
    void save_address(typecheck::expr_t::string_literal_t const&, llvm::Value* address);

    /**
     * @brief Store a new `value_t` for the given global symbol.
     * @return The iterator to the key-value pair with a flag which is true if insertion took place.
     */
    template <typename... Args>
    std::pair<iterator, bool> try_emplace(typecheck::expr_t::global_t name, Args&&... args)
    {
        return values.try_emplace(std::move(name), std::forward<Args>(args)...);
    }

private:
    struct eq_t { bool operator()(typecheck::expr_t const&, typecheck::expr_t const&) const; };

    std::size_t next_id = 0ul; /**< @brief Next unique ID returned from `get_next_id()`. */
    scope_map<typecheck::expr_t::global_t, value_t> values; /**< @brief Global functions, type definitions, etc. */

    std::map<typecheck::expr_t::global_t, llvm::Value*> global_addresses;
    std::map<typecheck::expr_t::string_literal_t, llvm::Value*> string_literal_addresses;

    /**
     * @brief LLVM global values associated to each unique string literal.
     * @note The comparator `std::less<>` allows look-up by `std::string_view`.
     */
    std::map<typecheck::expr_t::string_literal_t, llvm::Constant*> string_literals;

    /** @brief Stores the destructor to invoke for a non-array type. */
    std::unordered_map<typecheck::expr_t, llvm_func_t, std::hash<typecheck::expr_t>, eq_t> non_array_destructors;

    /** @brief Dedupe array destructors by passing the length as second argument and store them by element type. */
    std::unordered_map<typecheck::expr_t, llvm_func_t, std::hash<typecheck::expr_t>, eq_t> array_destructors;
};

/**
 * @brief Holds together things that have local visibility during IR codegen,
 * inside the current function scope or code block.
 *
 * It contains a `scope_map`, so it follows the same model of context extension and shadowing rules.
 *
 * It also stores the values that need to be destructed before leaving the scope associated to this object.
 */
struct local_ctx_t
{
    using value_t =
        std::variant<
            llvm::Value*,
            llvm_func_t
        >;

    local_ctx_t() = default;

#ifndef NDEBUG
    ~local_ctx_t() { assert(destructors.empty() and "local context would leak resources"); }
#endif

    /**
     * @brief Extend the current context, allowing new entries to shadow previous values and declarations.
     * @return A new context where shadowing can take place.
     */
    local_ctx_t extend() const;

    value_t* operator[](typecheck::expr_t::var_t const&);
    value_t const* operator[](typecheck::expr_t::var_t const&) const;

    template <typename... Args>
    auto try_emplace(typecheck::expr_t::var_t name, Args&&... args)
    {
        return entries.try_emplace(std::move(name), value_t{std::forward<Args>(args)...});
    }

    /** @brief Retrieve the address that was previously saved for the given variable, or `nullptr` if none saved yet. */
    llvm::Value* load_address(typecheck::expr_t::var_t const&) const;

    /** @brief Save the address where the value of the given variable is stored. */
    void save_address(typecheck::expr_t::var_t const&, llvm::Value* address);

    /**
     * @brief Values that need to be destructed before leaving the scope associated to this object.
     *
     * Each value is stored alongside its original type and new values are appended at the end.
     * If values need to be destroyed in reverse order it is the user responsibility to do so.
     */
    std::vector<std::pair<llvm::Value*, typecheck::expr_t>> destructors;

private:
    struct entry_t
    {
        value_t value;
        llvm::Value* address = nullptr;
    };
    scope_map<typecheck::expr_t::var_t, entry_t> entries;

    explicit local_ctx_t(scope_map<typecheck::expr_t::var_t, entry_t>);
};

} // namespace dep0::llvmgen
