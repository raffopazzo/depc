#include "dep0/llvmgen/gen.hpp"

#include "private/mangler.hpp"

#include "dep0/ast/substitute.hpp"

#include "dep0/digit_separator.hpp"
#include "dep0/fmap.hpp"
#include "dep0/match.hpp"
#include "dep0/scope_map.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>

#include <boost/range/adaptor/indexed.hpp>

#include <algorithm>
#include <ranges>

namespace dep0::llvmgen {

// Same as `llvm::FunctionCallee` but the latter has a broken API that doesn't allow access to the fields via `const&`.
struct llvm_func_t
{
    llvm::FunctionType* type;
    llvm::Value* func;

    explicit llvm_func_t(llvm::Function* const f) :
        type(f->getFunctionType()),
        func(f)
    { }

    llvm_func_t(llvm::FunctionType* const type, llvm::Value* const func) :
        type(type),
        func(func)
    { }
};

struct global_context_t
{
    llvm::LLVMContext& llvm_ctx;
    llvm::Module& llvm_module;
    scope_map<std::string, llvm_func_t> specialized_functions; // key is mangled name
    std::size_t next_id = 0ul;

    explicit global_context_t(llvm::Module& m) :
        llvm_ctx(m.getContext()),
        llvm_module(m)
    { }

    global_context_t(global_context_t const&) = delete;
    global_context_t(global_context_t&&) = delete;
    global_context_t& operator=(global_context_t const&) = delete;
    global_context_t& operator=(global_context_t&&) = delete;
};

struct local_context_t
{
    using value_t =
        std::variant<
            llvm::Value*,
            llvm_func_t,
            typecheck::type_def_t,
            typecheck::type_t
        >;

    local_context_t() = default;
    local_context_t extend() const
    {
        return local_context_t(values.extend());
    }

    value_t* operator[](ast::indexed_var_t const& k) { return values[k]; }
    value_t const* operator[](ast::indexed_var_t const& k) const { return values[k]; }

    template <typename... Args>
    auto try_emplace(ast::indexed_var_t name, Args&&... args)
    {
        return values.try_emplace(std::move(name), std::forward<Args>(args)...);
    }

private:
    scope_map<ast::indexed_var_t, value_t> values;

    explicit local_context_t(scope_map<ast::indexed_var_t, value_t> values) :
        values(std::move(values))
    { }
};

// Snippet is a model of a block of IR code that is being built,
// could be for example from the body of a function definition or the body of an if-else branch.
// It contains an entry point and possibly many open blocks, i.e. blocks which currently do not have
// a terminator. All open blocks are closed by jumping to the next statement in the parent scope.
// If required could also add last insert point in case we want to append to where we left off,
// which may or may not be the end iterator of the last open block.
struct snippet_t
{
    llvm::BasicBlock* entry_block;
    std::vector<llvm::BasicBlock*> open_blocks;

    template <typename F>
    void seal_open_blocks(llvm::IRBuilder<>& builder, F&& f)
    {
        for (auto& bb: open_blocks)
        {
            assert(not bb->getTerminator());
            builder.SetInsertPoint(bb);
            f(builder);
            assert(bb->getTerminator());
        }
        open_blocks.clear();
    }
};

struct llvm_func_proto_t
{
    struct arg_t
    {
        typecheck::type_t type;
        std::optional<typecheck::expr_t::var_t> var;
    };
    std::vector<arg_t> args;
    typecheck::type_t ret_type;
};

// all gen functions must be forward-declared here

static llvm_func_t gen_func(global_context_t&, local_context_t const&, typecheck::expr_t::abs_t const&);
static void gen_func(
    global_context_t&,
    local_context_t&,
    source_text const&,
    typecheck::expr_t::abs_t const&);

// TODO at the moment this is not used but we should find a way to use it again
static llvm_func_t gen_specialized_func(
    global_context_t&,
    local_context_t&,
    std::string mangled_name,
    typecheck::expr_t::abs_t const&,
    std::vector<typecheck::expr_t> const&);

static snippet_t gen_body(
    global_context_t&,
    local_context_t const&,
    typecheck::body_t const&,
    std::string_view,
    llvm::Function*);
static void gen_stmt(
    global_context_t&,
    local_context_t const&,
    snippet_t&,
    llvm::IRBuilder<>&,
    typecheck::stmt_t const&,
    llvm::Function*);
static llvm::Type* gen_type(global_context_t&, local_context_t const&, typecheck::type_t const&);
static llvm::Value* gen_val(global_context_t&, local_context_t const&, llvm::IRBuilder<>& , typecheck::expr_t const&);
static llvm::CallInst* gen_func_call(
    global_context_t&,
    local_context_t const&,
    llvm::IRBuilder<>&,
    typecheck::expr_t::app_t const&);
static llvm::CallInst* gen_func_call(
    global_context_t&,
    local_context_t const&,
    llvm::IRBuilder<>&,
    llvm_func_t const&,
    std::vector<typecheck::expr_t> const&);
static llvm::FunctionType* gen_func_type(global_context_t&, local_context_t const&, llvm_func_proto_t const&);
static void gen_func_attributes(local_context_t const&, llvm_func_proto_t const&, llvm::Function*);
static void gen_func_args(local_context_t&, llvm_func_proto_t const&, llvm::Function*);
static void gen_func_body(
    global_context_t&,
    local_context_t const&,
    llvm_func_proto_t const&,
    typecheck::body_t const&,
    llvm::Function*);

static bool is_first_order_abstraction(typecheck::expr_t::abs_t const&);
static bool is_first_order_application(typecheck::expr_t::app_t const&);
static bool is_first_order_type(typecheck::type_t const&);
static bool is_first_order_function_type(typecheck::type_t::arr_t const&);

bool is_first_order_abstraction(typecheck::expr_t::abs_t const& f)
{
    return is_first_order_type(f.ret_type) and
        std::ranges::all_of(
            f.args,
            [] (typecheck::func_arg_t const& arg)
            {
                auto const term = std::get_if<typecheck::func_arg_t::term_arg_t>(&arg.value);
                return term and is_first_order_type(term->type);
            });
}

bool is_first_order_application(typecheck::expr_t::app_t const& f)
{
    return std::ranges::all_of(f.args, [] (auto const& x) { return ast::is_type(x.properties.sort); });
}

bool is_first_order_type(typecheck::type_t const& type)
{
    return match(
        type.value,
        [] (typecheck::type_t::arr_t const& t) { return is_first_order_function_type(t); },
        [] (auto const&) { return true; });
}

bool is_first_order_function_type(typecheck::type_t::arr_t const& x)
{
    return is_first_order_type(x.ret_type.get()) and
        std::ranges::all_of(
            x.args,
            [](typecheck::func_arg_t const& arg)
            {
                auto const term = std::get_if<typecheck::func_arg_t::term_arg_t>(&arg.value);
                return term and is_first_order_type(term->type);
            });
}

expected<unique_ref<llvm::Module>> gen(
    llvm::LLVMContext& llvm_ctx,
    std::string_view const name,
    typecheck::module_t const& m
) noexcept
{
    auto llvm_module = make_ref<llvm::Module>(name, llvm_ctx);
    global_context_t global(llvm_module.get());
    local_context_t local;
    // TODO: allow declarations in any order with 2 pass (both type and function defs)
    for (auto const& def: m.type_defs)
    {
        auto const& name = match(def.value, [] (auto const& x) -> auto const& { return x.name; });
        bool const inserted = local.try_emplace(ast::indexed_var_t{name}, def).second;
        assert(inserted);
    }
    for (auto const& def: m.func_defs)
    {
        // LLVM can only generate functions for 1st order abstractions;
        // for 2nd order abstractions we rely on beta-delta normalization to produce a value or a 1st order application
        if (is_first_order_abstraction(def.value))
            gen_func(global, local, def.name, def.value);
    }

    std::string err;
    llvm::raw_string_ostream ostream(err);
    return llvm::verifyModule(llvm_module.get(), &ostream) // yes true means false...
        ? expected<unique_ref<llvm::Module>>{error_t{err}}
        : expected<unique_ref<llvm::Module>>{std::in_place, std::move(llvm_module)};
}

// all gen functions must be implemented here
llvm::Type* gen_type(global_context_t& global, local_context_t const& local, typecheck::type_t const& x)
{
    return match(
        x.value,
        [&] (typecheck::type_t::bool_t const&) -> llvm::Type* { return llvm::Type::getInt1Ty(global.llvm_ctx); },
        // this may or may not be fine depending on how LLVM defines `void` (i.e. what properties it has)
        // for instance, what does it mean to have an array of `void` types in LLVM?
        [&] (typecheck::type_t::unit_t const&) -> llvm::Type* { return llvm::Type::getVoidTy(global.llvm_ctx); },
        [&] (typecheck::type_t::i8_t const&) -> llvm::Type* { return llvm::Type::getInt8Ty(global.llvm_ctx); },
        [&] (typecheck::type_t::i16_t const&) -> llvm::Type* { return llvm::Type::getInt16Ty(global.llvm_ctx); },
        [&] (typecheck::type_t::i32_t const&) -> llvm::Type* { return llvm::Type::getInt32Ty(global.llvm_ctx); },
        [&] (typecheck::type_t::i64_t const&) -> llvm::Type* { return llvm::Type::getInt64Ty(global.llvm_ctx); },
        [&] (typecheck::type_t::u8_t const&) -> llvm::Type* { return llvm::Type::getInt8Ty(global.llvm_ctx); },
        [&] (typecheck::type_t::u16_t const&) -> llvm::Type* { return llvm::Type::getInt16Ty(global.llvm_ctx); },
        [&] (typecheck::type_t::u32_t const&) -> llvm::Type* { return llvm::Type::getInt32Ty(global.llvm_ctx); },
        [&] (typecheck::type_t::u64_t const&) -> llvm::Type* { return llvm::Type::getInt64Ty(global.llvm_ctx); },
        [&] (typecheck::type_t::var_t const& var) -> llvm::Type*
        {
            auto const val = local[var.name];
            assert(val and "unknown type");
            return match(
                *val,
                [] (llvm::Value*) -> llvm::Type*
                {
                    assert(false and "found a value but was expecting a type");
                    __builtin_unreachable();
                },
                [] (llvm_func_t const&) -> llvm::Type*
                {
                    assert(false and "found a function but was expecting a type");
                    __builtin_unreachable();
                },
                [&] (typecheck::type_def_t const& t)
                {
                    return match(
                        t.value,
                        [&] (typecheck::type_def_t::integer_t const& integer) -> llvm::Type*
                        {
                            switch (integer.width)
                            {
                            case ast::width_t::_8: return llvm::Type::getInt8Ty(global.llvm_ctx);
                            case ast::width_t::_16: return llvm::Type::getInt16Ty(global.llvm_ctx);
                            case ast::width_t::_32: return llvm::Type::getInt32Ty(global.llvm_ctx);
                            case ast::width_t::_64: return llvm::Type::getInt64Ty(global.llvm_ctx);
                            default: __builtin_unreachable();
                            }
                        });
                },
                [&] (typecheck::type_t const& type)
                {
                    return gen_type(global, local, type);
                });
        },
        [&] (typecheck::type_t::arr_t const& x) -> llvm::Type*
        {
            assert(is_first_order_function_type(x) and "can only generate an llvm type for 1st order function types");
            bool constexpr is_var_arg = false;
            auto const func_type =
                llvm::FunctionType::get(
                    gen_type(global, local, x.ret_type.get()),
                    fmap(
                        x.args,
                        [&] (typecheck::func_arg_t const& arg)
                        {
                            return gen_type(global, local, std::get<typecheck::func_arg_t::term_arg_t>(arg.value).type);
                        }),
                    is_var_arg);
            return func_type->getPointerTo();
        });
}

llvm::Attribute::AttrKind get_sign_ext_attribute(local_context_t const& local, typecheck::type_t const& x)
{
    return match(
        x.value,
        [] (typecheck::type_t::bool_t const&) { return llvm::Attribute::None; },
        [] (typecheck::type_t::unit_t const&) { return llvm::Attribute::None; },
        [] (typecheck::type_t::i8_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::type_t::i16_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::type_t::i32_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::type_t::i64_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::type_t::u8_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::type_t::u16_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::type_t::u32_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::type_t::u64_t const&) { return llvm::Attribute::ZExt; },
        [&] (typecheck::type_t::var_t const& var)
        {
            auto const val = local[var.name];
            assert(val and "unknown type");
            return match(
                *val,
                [] (llvm::Value*) -> llvm::Attribute::AttrKind
                {
                    assert(false and "found a value but was expecting a type");
                    __builtin_unreachable();
                },
                [] (llvm_func_t const&) -> llvm::Attribute::AttrKind
                {
                    assert(false and "found a function but was expecting a type");
                    __builtin_unreachable();
                },
                [&] (typecheck::type_def_t const& t)
                {
                    return match(
                        t.value,
                        [&] (typecheck::type_def_t::integer_t const& x)
                        {
                            return x.sign == ast::sign_t::signed_v ? llvm::Attribute::SExt : llvm::Attribute::ZExt;
                        });
                },
                [&] (typecheck::type_t const& type)
                {
                    return get_sign_ext_attribute(local, type);
                });
        },
        [] (typecheck::type_t::arr_t const&) { return llvm::Attribute::None; });
}

llvm::FunctionType* gen_func_type(
    global_context_t& global,
    local_context_t const& local,
    llvm_func_proto_t const& proto)
{
    bool constexpr is_var_arg = false;
    return llvm::FunctionType::get(
        gen_type(global, local, proto.ret_type),
        fmap(proto.args, [&] (auto const& arg) { return gen_type(global, local, arg.type); }),
        is_var_arg);
}

void gen_func_args(local_context_t& local, llvm_func_proto_t const& proto, llvm::Function* const llvm_f)
{
    for (auto const i: std::views::iota(0ul, proto.args.size()))
    {
        auto* const llvm_arg = llvm_f->getArg(i);
        if (auto const attr = get_sign_ext_attribute(local, proto.args[i].type); attr != llvm::Attribute::None)
            llvm_arg->addAttr(attr);
        if (proto.args[i].var)
        {
            if (proto.args[i].var->name.idx == 0ul)
                llvm_arg->setName(proto.args[i].var->name.txt.view());
            bool inserted = false;
            if (std::holds_alternative<typecheck::type_t::arr_t>(proto.args[i].type.value))
            {
                assert(llvm_arg->getType()->isPointerTy());
                auto const function_type = cast<llvm::FunctionType>(llvm_arg->getType()->getPointerElementType());
                assert(function_type);
                inserted = local.try_emplace(proto.args[i].var->name, llvm_func_t(function_type, llvm_arg)).second;
            }
            else
                inserted = local.try_emplace(proto.args[i].var->name, llvm_arg).second;
            assert(inserted);
        }
    }
}

void gen_func_attributes(local_context_t const& local, llvm_func_proto_t const& proto, llvm::Function* const llvm_f)
{
    if (auto const attr = get_sign_ext_attribute(local, proto.ret_type); attr != llvm::Attribute::None)
        llvm_f->addAttribute(llvm::AttributeList::ReturnIndex, attr);
}

void gen_func_body(
    global_context_t& global,
    local_context_t const& local,
    llvm_func_proto_t const& proto,
    typecheck::body_t const& body,
    llvm::Function* const llvm_f)
{
    auto snippet = gen_body(global, local, body, "entry", llvm_f);
    if (snippet.open_blocks.size() and std::holds_alternative<typecheck::type_t::unit_t>(proto.ret_type.value))
    {
        auto builder = llvm::IRBuilder<>(global.llvm_ctx);
        snippet.seal_open_blocks(builder, [] (auto& builder) { builder.CreateRetVoid(); });
    }
}

llvm_func_t gen_func(
    global_context_t& global,
    local_context_t const& local,
    typecheck::expr_t::abs_t const& f)
{
    assert(is_first_order_abstraction(f) and "can only generate an llvm function for 1st order abstractions");
    auto const proto =
        llvm_func_proto_t{
            fmap(
                f.args,
                [] (auto const& arg)
                {
                    auto const& term = std::get<typecheck::func_arg_t::term_arg_t>(arg.value);
                    return llvm_func_proto_t::arg_t{term.type, term.var};
                }),
            f.ret_type};
    auto const name = std::string{"$_func_"} + std::to_string(global.next_id++);
    auto const llvm_f =
        llvm::Function::Create(
            gen_func_type(global, local, proto),
            llvm::Function::PrivateLinkage,
            name,
            global.llvm_module);
    auto f_ctx = local.extend();
    gen_func_args(f_ctx, proto, llvm_f);
    gen_func_attributes(f_ctx, proto, llvm_f);
    gen_func_body(global, f_ctx, proto, f.body, llvm_f);
    return llvm_func_t(llvm_f);
}

void gen_func(
    global_context_t& global,
    local_context_t& local,
    source_text const& name,
    typecheck::expr_t::abs_t const& f)
{
    assert(is_first_order_abstraction(f) and "can only generate an llvm function for 1st order abstractions");
    auto const proto =
        llvm_func_proto_t{
            fmap(
                f.args,
                [] (auto const& arg)
                {
                    auto const& term = std::get<typecheck::func_arg_t::term_arg_t>(arg.value);
                    return llvm_func_proto_t::arg_t{term.type, term.var};
                }),
            f.ret_type};
    auto const llvm_f =
        llvm::Function::Create(
            gen_func_type(global, local, proto),
            llvm::Function::ExternalLinkage,
            name.view(),
            global.llvm_module);
    // add function to the parent context; but generation must happen in the function context
    bool const inserted = local.try_emplace(ast::indexed_var_t{name}, llvm_func_t(llvm_f)).second;
    assert(inserted);
    auto f_ctx = local.extend();
    gen_func_args(f_ctx, proto, llvm_f);
    gen_func_attributes(f_ctx, proto, llvm_f);
    gen_func_body(global, f_ctx, proto, f.body, llvm_f);
}

llvm_func_t gen_specialized_func(
    global_context_t& global,
    local_context_t const& local,
    std::string mangled_name,
    typecheck::expr_t::abs_t const& f,
    std::vector<typecheck::expr_t> const& args)
{
    assert(f.args.size() == args.size());
    llvm_func_proto_t proto{{}, f.ret_type};
    proto.args.reserve(args.size());
    auto f_ctx = local.extend();
    for (auto const& [i, arg]: boost::adaptors::index(args))
        match(
            f.args[i].value,
            [&] (typecheck::func_arg_t::type_arg_t const& type_arg)
            {
                auto const& arg_type = std::get<typecheck::type_t>(arg.value);
                if (type_arg.var)
                {
                    bool const inserted = f_ctx.try_emplace(type_arg.var->name, arg_type).second;
                    assert(inserted);
                    // NB: in general it is not correct to perform substitution directly in the return type,
                    // because some later argument could in theory rebind the current type variable;
                    // but we forbid this kind of shadowing, so in this case it is fine to substitute directly.
                    substitute(proto.ret_type, *type_arg.var, arg_type);
                }
            },
            [&] (typecheck::func_arg_t::term_arg_t const& term_arg)
            {
                proto.args.emplace_back(std::get<typecheck::type_t>(arg.properties.sort), term_arg.var);
            });
    auto const llvm_f =
        llvm::Function::Create(
            gen_func_type(global, local, proto),
            llvm::Function::PrivateLinkage,
            mangled_name,
            global.llvm_module);
    // add specialization to global context; but generation must happen in the function context
    auto const [it, inserted] = global.specialized_functions.try_emplace(std::move(mangled_name), llvm_func_t(llvm_f));
    assert(inserted);
    gen_func_args(f_ctx, proto, llvm_f);
    gen_func_attributes(f_ctx, proto, llvm_f);
    gen_func_body(global, f_ctx, proto, f.body, llvm_f);
    return it->second;
}

snippet_t gen_body(
    global_context_t& global,
    local_context_t const& local,
    typecheck::body_t const& body,
    std::string_view const entry_block_name,
    llvm::Function* const llvm_f)
{
    snippet_t snippet;
    auto builder = llvm::IRBuilder<>(global.llvm_ctx);
    builder.SetInsertPoint(snippet.entry_block = llvm::BasicBlock::Create(global.llvm_ctx, entry_block_name, llvm_f));
    auto it = body.stmts.begin();
    if (it == body.stmts.end())
        snippet.open_blocks.push_back(snippet.entry_block);
    else
    {
        gen_stmt(global, local, snippet, builder, *it, llvm_f);
        while (++it != body.stmts.end())
        {
            if (snippet.open_blocks.size())
            {
                auto next = llvm::BasicBlock::Create(global.llvm_ctx, "next", llvm_f);
                snippet.seal_open_blocks(builder, [next] (auto& builder) { builder.CreateBr(next); });
                builder.SetInsertPoint(next);
            }
            gen_stmt(global, local, snippet, builder, *it, llvm_f);
        }
    }
    return snippet;
}

void gen_stmt(
    global_context_t& global,
    local_context_t const& local,
    snippet_t& snipppet,
    llvm::IRBuilder<>& builder,
    typecheck::stmt_t const& stmt,
    llvm::Function* const llvm_f)
{
    match(
        stmt.value,
        [&] (typecheck::expr_t::app_t const& x)
        {
            gen_func_call(global, local, builder, x);
        },
        [&] (typecheck::stmt_t::if_else_t const& x)
        {
            auto const cond = gen_val(global, local, builder, x.cond);
            auto const then = gen_body(global, local, x.true_branch, "then", llvm_f);
            std::ranges::copy(then.open_blocks, std::back_inserter(snipppet.open_blocks));
            if (x.false_branch)
            {
                auto const else_ = gen_body(global, local, *x.false_branch, "else", llvm_f);
                std::ranges::copy(else_.open_blocks, std::back_inserter(snipppet.open_blocks));
                builder.CreateCondBr(cond, then.entry_block, else_.entry_block);
            }
            else
            {
                auto const else_ = llvm::BasicBlock::Create(global.llvm_ctx, "else", llvm_f);
                builder.CreateCondBr(cond, then.entry_block, else_);
                snipppet.open_blocks.push_back(else_);
            }
        },
        [&] (typecheck::stmt_t::return_t const& x)
        {
            if (x.expr)
                builder.CreateRet(gen_val(global, local, builder, *x.expr));
            else
                builder.CreateRetVoid();
        });
}

llvm::Value* gen_val(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t const& expr)
{
    auto* const type = std::get_if<typecheck::type_t>(&expr.properties.sort);
    assert(type and "cannot generate a value for a type variable");
    return match(
        expr.value,
        [&] (typecheck::expr_t::arith_expr_t const& x) -> llvm::Value*
        {
            return match(
                x.value,
                [&] (typecheck::expr_t::arith_expr_t::plus_t const& x) -> llvm::Value*
                {
                    // TODO add NUW/NSW but probably depends on decision whether typechecking `plus_t` requires proofs?
                    return builder.CreateAdd(
                        gen_val(global, local, builder, x.lhs.get()),
                        gen_val(global, local, builder, x.rhs.get()));
                });
        },
        [&] (typecheck::expr_t::boolean_constant_t const& x) -> llvm::Value*
        {
            return llvm::ConstantInt::getBool(global.llvm_ctx, x.value);
        },
        [&] (typecheck::expr_t::numeric_constant_t const& x) -> llvm::Value*
        {
            std::optional<std::string> tmp;
            std::string_view number;
            if (x.sign or contains_digit_separator(x.number))
            {
                auto& s = tmp.emplace();
                s.reserve(x.number.size() + 1);
                if (x.sign)
                    s.push_back(*x.sign);
                remove_digit_separator(x.number, s);
                number = s;
            }
            else
                number = x.number;
            auto const& type = std::get<typecheck::type_t>(expr.properties.sort);
            auto const llvm_type = cast<llvm::IntegerType>(gen_type(global, local, type));
            return llvm::ConstantInt::get(llvm_type, number, 10);
        },
        [&] (typecheck::expr_t::var_t const& var) -> llvm::Value*
        {
            auto const val = local[var.name];
            assert(val and "unknown variable");
            return match(
                *val,
                [] (llvm::Value* const p) { return p; },
                [] (llvm_func_t const& c) { return c.func; },
                [] (typecheck::type_def_t const&) -> llvm::Value*
                {
                    assert(false and "found a typedef but was expecting a value");
                    __builtin_unreachable();
                },
                [] (typecheck::type_t const&) -> llvm::Value*
                {
                    assert(false and "found a type but was expecting a value");
                    __builtin_unreachable();
                });
        },
        [&] (typecheck::expr_t::app_t const& x) -> llvm::Value*
        {
            // TODO could perhaps generate a snippet for immediately invoked lambdas
            return gen_func_call(global, local, builder, x);
        },
        [&] (typecheck::expr_t::abs_t const&) -> llvm::Value*
        {
            assert(false and "Generation of abstraction expression not yet implemented");
            __builtin_unreachable();
        },
        [&] (typecheck::type_t const&) -> llvm::Value*
        {
            assert(false and "Generation of an llvm value from a type expression is not possible");
            __builtin_unreachable();
        });
}

llvm::CallInst* gen_func_call(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t::app_t const& app)
{
    auto const func =
        match(
            app.func.get().value,
            [&] (typecheck::expr_t::var_t const& var)
            {
                auto const val = local[var.name];
                assert(val and "unknown function call");
                return match(
                    *val,
                    [] (llvm::Value*) -> llvm_func_t
                    {
                        assert(false and "found a value but was expecting a function");
                        __builtin_unreachable();
                    },
                    [] (llvm_func_t const& func) -> llvm_func_t
                    {
                        return func;
                    },
                    [] (typecheck::type_def_t const&) -> llvm_func_t
                    {
                        assert(false and "found a typedef but was expecting a function");
                        __builtin_unreachable();
                    },
                    [] (typecheck::type_t const&) -> llvm_func_t
                    {
                        assert(false and "found a type but was expecting a function");
                        __builtin_unreachable();
                    });
            },
            [&] (typecheck::expr_t::abs_t const& abs) -> llvm_func_t
            {
                return gen_func(global, local, abs);
            },
            [] (auto const&) -> llvm_func_t
            {
                assert(false and "unexpected invocable expression");
                __builtin_unreachable();
            });
    // this could be a direct call to a global function, or an indirect call via a function pointer
    return gen_func_call(global, local, builder, func, app.args);
}

llvm::CallInst* gen_func_call(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    llvm_func_t const& callee,
    std::vector<typecheck::expr_t> const& args)
{
    assert(
        std::ranges::all_of(args, [] (auto const& x) { return ast::is_type(x.properties.sort); })
        and "can only generate function call for 1st order applications");
    auto const call =
        builder.CreateCall(
            callee.type,
            callee.func,
            fmap(args, [&] (auto const& arg)
            {
                return gen_val(global, local, builder, arg);
            }));
    for (auto const i: std::views::iota(0ul, args.size()))
    {
        auto const& arg_type = std::get<typecheck::type_t>(args[i].properties.sort);
        if (auto const attr = get_sign_ext_attribute(local, arg_type); attr != llvm::Attribute::None)
            call->addParamAttr(i, attr);
    }
    return call;
}

}
