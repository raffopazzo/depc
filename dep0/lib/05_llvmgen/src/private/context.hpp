#pragma once

#include "private/llvm_func.hpp"

#include "dep0/typecheck/ast.hpp"

#include "dep0/scope_map.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include <variant>

namespace dep0::llvmgen {

/**
 * Holds together things that have global visibility during IR codegen.
 */
struct global_ctx_t
{
    using value_t =
        std::variant<
            llvm_func_t,
            typecheck::type_def_t
        >;

    /**
     * The LLVM Context being used during IR codegen.
     */
    llvm::LLVMContext& llvm_ctx;

    /**
     * The LLVM module for which IR is being codegen.
     */
    llvm::Module& llvm_module;

    /**
     * Obtain a unique ID, useful for example to generate a unique name for anonymous function.
     */
    std::size_t get_next_id();

    explicit global_ctx_t(llvm::Module&);

    global_ctx_t(global_ctx_t const&) = delete;
    global_ctx_t(global_ctx_t&&) = delete;
    global_ctx_t& operator=(global_ctx_t const&) = delete;
    global_ctx_t& operator=(global_ctx_t&&) = delete;

    value_t* operator[](typecheck::expr_t::global_t const&);
    value_t const* operator[](typecheck::expr_t::global_t const&) const;

    /**
     * Return the LLVM global value holding the given string literal,
     * or nullptr if no global value exists yet.
     */
    llvm::Value* get_string_literal(std::string_view);

    /**
     * Store an LLVM global value holding the given string literal;
     * if one already exists, it will be overwritten.
     */
    void store_string_literal(std::string, llvm::Value*);

    template <typename... Args>
    auto try_emplace(typecheck::expr_t::global_t name, Args&&... args)
    {
        return values.try_emplace(std::move(name), std::forward<Args>(args)...);
    }

private:
    /**
     * Holds the next unique ID that will be assigned by `get_next_id()`.
     */
    std::size_t next_id = 0ul;

    /**
     * Holds global objects, for example functions and type-defs.
     */
    scope_map<typecheck::expr_t::global_t, value_t> values;

    /**
     * Holds the LLVM global value associated to each unique string literal.
     * @note The comparator `std::less<>` is required to allow look-up with `string_view`.
     */
    std::map<std::string, llvm::Value*, std::less<>> string_literals;
};

/**
 * Holds together things that have local visibility during IR codegen,
 * for example inside the current function scope or code block.
 * 
 * It contains a `scope_map`, so it follows the same model of context extension and shadowing rules.
 */
struct local_ctx_t
{
    using value_t =
        std::variant<
            llvm::Value*,
            llvm_func_t
        >;

    local_ctx_t() = default;

    /**
     * Extend the current context, allowing new enties to be stored even with colliding names,
     * thereby shadowing previous values and declarations.
     *
     * @return A new context where shadowing can take place.
     */
    local_ctx_t extend() const;

    value_t* operator[](typecheck::expr_t::var_t const&);
    value_t const* operator[](typecheck::expr_t::var_t const&) const;

    template <typename... Args>
    auto try_emplace(typecheck::expr_t::var_t name, Args&&... args)
    {
        return values.try_emplace(std::move(name), std::forward<Args>(args)...);
    }

private:
    scope_map<typecheck::expr_t::var_t, value_t> values;

    explicit local_ctx_t(scope_map<typecheck::expr_t::var_t, value_t>);
};

} // namespace dep0::llvmgen
