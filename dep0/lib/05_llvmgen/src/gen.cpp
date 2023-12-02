#include "dep0/llvmgen/gen.hpp"

#include "dep0/digit_separator.hpp"
#include "dep0/fmap.hpp"
#include "dep0/match.hpp"
#include "dep0/scope_map.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>

#include <boost/range/adaptor/indexed.hpp>

#include <algorithm>
#include <optional>
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
            typecheck::type_def_t
        >;

    local_context_t() = default;
    local_context_t extend() const
    {
        return local_context_t(values.extend());
    }

    value_t* operator[](typecheck::expr_t::var_t const& k) { return values[k]; }
    value_t const* operator[](typecheck::expr_t::var_t const& k) const { return values[k]; }

    template <typename... Args>
    auto try_emplace(typecheck::expr_t::var_t name, Args&&... args)
    {
        return values.try_emplace(std::move(name), std::forward<Args>(args)...);
    }

private:
    scope_map<typecheck::expr_t::var_t, value_t> values;

    explicit local_context_t(scope_map<typecheck::expr_t::var_t, value_t> values) :
        values(std::move(values))
    { }
};

// Snippet is a model of a block of IR code that is being built,
// could be for example from the body of a function definition or the body of an if-else branch.
// It contains an entry point and possibly many open blocks, i.e. blocks currently without a terminator.
struct snippet_t
{
    llvm::BasicBlock* entry_block;
    std::vector<llvm::BasicBlock*> open_blocks;

    // TODO if required could also add last insert point in case we want to append to where we left off

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

// Proof that the pointed `pi_t` is a 1st order function type.
// Non-copyable as we don't intend to pass it by-value or store it, but only to use it as proof object.
// Must me movable in order to construct an `optional<llvm_func_proto_t>`;
// this means that in reality you can actually pass/store it if you try hard enough; please don't do that.
class llvm_func_proto_t
{
    std::vector<typecheck::func_arg_t> m_args;
    typecheck::expr_t m_ret_type;

    llvm_func_proto_t(typecheck::expr_t::pi_t const& x)  : m_args(x.args), m_ret_type(x.ret_type.get()) { }
    llvm_func_proto_t(typecheck::expr_t::abs_t const& x) : m_args(x.args), m_ret_type(x.ret_type.get()) { }
    llvm_func_proto_t(llvm_func_proto_t const&) = delete;
    llvm_func_proto_t& operator=(llvm_func_proto_t const&) = delete;

public:
    llvm_func_proto_t(llvm_func_proto_t&&) = default;
    llvm_func_proto_t& operator=(llvm_func_proto_t&&) = default;

    static std::optional<llvm_func_proto_t> from_pi(typecheck::expr_t::pi_t const&);
    static std::optional<llvm_func_proto_t> from_abs(typecheck::expr_t::abs_t const&);

    std::vector<typecheck::func_arg_t> const& args() const { return m_args; }
    typecheck::func_arg_t const& arg(std::size_t const i) const { return m_args[i]; }
    typecheck::expr_t const& ret_type() const { return m_ret_type; }
};

// all gen functions must be forward-declared here

static llvm_func_t gen_func(
    global_context_t&,
    local_context_t const&,
    llvm_func_proto_t const&,
    typecheck::expr_t::abs_t const&);
static void gen_func(
    global_context_t&,
    local_context_t&,
    source_text const&,
    llvm_func_proto_t const&,
    typecheck::expr_t::abs_t const&);

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
static llvm::Type* gen_type(global_context_t&, local_context_t const&, typecheck::expr_t const&);
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

static bool is_first_order_function_type(std::vector<typecheck::func_arg_t> const&, typecheck::expr_t const& ret_type);
static bool is_first_order_type(typecheck::expr_t const&);

std::optional<llvm_func_proto_t> llvm_func_proto_t::from_pi(typecheck::expr_t::pi_t const& x)
{
    return is_first_order_function_type(x.args, x.ret_type.get()) ? std::optional{llvm_func_proto_t(x)} : std::nullopt;
}

std::optional<llvm_func_proto_t> llvm_func_proto_t::from_abs(typecheck::expr_t::abs_t const& x)
{
    return is_first_order_function_type(x.args, x.ret_type.get()) ? std::optional{llvm_func_proto_t(x)} : std::nullopt;
}

bool is_first_order_function_type(std::vector<typecheck::func_arg_t> const& args, typecheck::expr_t const& ret_type)
{
    return
        std::ranges::all_of(args, [] (typecheck::func_arg_t const& arg) { return is_first_order_type(arg.type); })
        and is_first_order_type(ret_type);
}

bool is_first_order_type(typecheck::expr_t const& type)
{
    return match(
        type.value,
        [] (typecheck::expr_t::typename_t const&) { return false; },
        [] (typecheck::expr_t::bool_t const&) { return true; },
        [] (typecheck::expr_t::unit_t const&) { return true; },
        [] (typecheck::expr_t::i8_t const&) { return true; },
        [] (typecheck::expr_t::i16_t const&) { return true; },
        [] (typecheck::expr_t::i32_t const&) { return true; },
        [] (typecheck::expr_t::i64_t const&) { return true; },
        [] (typecheck::expr_t::u8_t const&) { return true; },
        [] (typecheck::expr_t::u16_t const&) { return true; },
        [] (typecheck::expr_t::u32_t const&) { return true; },
        [] (typecheck::expr_t::u64_t const&) { return true; },
        [] (typecheck::expr_t::boolean_constant_t const&) { return false; },
        [] (typecheck::expr_t::numeric_constant_t const&) { return false; },
        [] (typecheck::expr_t::arith_expr_t const&) { return false; },
        [] (typecheck::expr_t::var_t const&) { return true; }, // caller must call only if expr is a type
        [] (typecheck::expr_t::app_t const&) { return false; },
        [] (typecheck::expr_t::abs_t const&) { return false; },
        [] (typecheck::expr_t::pi_t const &t) { return is_first_order_function_type(t.args, t.ret_type.get()); });
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
        bool const inserted = local.try_emplace(typecheck::expr_t::var_t{name}, def).second;
        assert(inserted);
    }
    for (auto const& def: m.func_defs)
    {
        // LLVM can only generate functions for 1st order abstractions;
        // for 2nd order abstractions we rely on beta-delta normalization to produce a value or a 1st order application;
        // also note that we could in theory check `def.properites.sort` as a pi-type instead of using `from_abs()` but
        // currently beta-delta normalization operates on the templated AST, so it does not reduce inside `properties`
        if (auto proto = llvm_func_proto_t::from_abs(def.value))
            gen_func(global, local, def.name, *proto, def.value);
    }

    std::string err;
    llvm::raw_string_ostream ostream(err);
    return llvm::verifyModule(llvm_module.get(), &ostream) // yes true means false...
        ? expected<unique_ref<llvm::Module>>{error_t{err}}
        : expected<unique_ref<llvm::Module>>{std::in_place, std::move(llvm_module)};
}

// all gen functions must be implemented here
llvm::Type* gen_type(global_context_t& global, local_context_t const& local, typecheck::expr_t const& x)
{
    return match(
        x.value,
        [&] (typecheck::expr_t::typename_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for typename");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::bool_t const&) -> llvm::Type* { return llvm::Type::getInt1Ty(global.llvm_ctx); },
        // this may or may not be fine depending on how LLVM defines `void` (i.e. what properties it has)
        // for instance, what does it mean to have an array of `void` types in LLVM?
        [&] (typecheck::expr_t::unit_t const&) -> llvm::Type* { return llvm::Type::getVoidTy(global.llvm_ctx); },
        [&] (typecheck::expr_t::i8_t const&) -> llvm::Type* { return llvm::Type::getInt8Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::i16_t const&) -> llvm::Type* { return llvm::Type::getInt16Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::i32_t const&) -> llvm::Type* { return llvm::Type::getInt32Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::i64_t const&) -> llvm::Type* { return llvm::Type::getInt64Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::u8_t const&) -> llvm::Type* { return llvm::Type::getInt8Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::u16_t const&) -> llvm::Type* { return llvm::Type::getInt16Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::u32_t const&) -> llvm::Type* { return llvm::Type::getInt32Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::u64_t const&) -> llvm::Type* { return llvm::Type::getInt64Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::boolean_constant_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a boolean constant");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::numeric_constant_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a numeric constant");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::arith_expr_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for an arithmetic expression");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::var_t const& var) -> llvm::Type*
        {
            auto const val = local[var];
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
                });
        },
        [&] (typecheck::expr_t::app_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a function application");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::abs_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a function abstraction");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::pi_t const& x) -> llvm::Type*
        {
            auto proto = llvm_func_proto_t::from_pi(x);
            assert(proto and "can only generate an llvm type for 1st order function types");
            return gen_func_type(global, local, *proto)->getPointerTo();
        });
}

llvm::Attribute::AttrKind get_sign_ext_attribute(local_context_t const& local, typecheck::expr_t const& x)
{
    return match(
        x.value,
        [] (typecheck::expr_t::typename_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::bool_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::unit_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::i8_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::expr_t::i16_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::expr_t::i32_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::expr_t::i64_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::expr_t::u8_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::expr_t::u16_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::expr_t::u32_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::expr_t::u64_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::expr_t::u64_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::expr_t::boolean_constant_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::numeric_constant_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::arith_expr_t const&) { return llvm::Attribute::None; },
        [&] (typecheck::expr_t::var_t const& var)
        {
            auto const val = local[var];
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
                });
        },
        [] (typecheck::expr_t::app_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::abs_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::pi_t const&) { return llvm::Attribute::None; });
}

llvm::FunctionType* gen_func_type(
    global_context_t& global,
    local_context_t const& local,
    llvm_func_proto_t const& proto)
{
    bool constexpr is_var_arg = false;
    return llvm::FunctionType::get(
        gen_type(global, local, proto.ret_type()),
        fmap(proto.args(), [&] (auto const& arg) { return gen_type(global, local, arg.type); }),
        is_var_arg);
}

void gen_func_args(local_context_t& local, llvm_func_proto_t const& proto, llvm::Function* const llvm_f)
{
    for (auto const i: std::views::iota(0ul, proto.args().size()))
    {
        auto const llvm_arg = llvm_f->getArg(i);
        auto const [arg_type, arg_var] = std::tie(proto.arg(i).type, proto.arg(i).var);
        if (auto const attr = get_sign_ext_attribute(local, arg_type); attr != llvm::Attribute::None)
            llvm_arg->addAttr(attr);
        if (arg_var)
        {
            if (arg_var->idx == 0ul)
                llvm_arg->setName(arg_var->name.view());
            bool inserted = false;
            if (std::holds_alternative<typecheck::expr_t::pi_t>(arg_type.value))
            {
                assert(llvm_arg->getType()->isPointerTy());
                auto const function_type = cast<llvm::FunctionType>(llvm_arg->getType()->getPointerElementType());
                assert(function_type);
                inserted = local.try_emplace(*arg_var, llvm_func_t(function_type, llvm_arg)).second;
            }
            else
                inserted = local.try_emplace(*arg_var, llvm_arg).second;
            assert(inserted);
        }
    }
}

void gen_func_attributes(local_context_t const& local, llvm_func_proto_t const& proto, llvm::Function* const llvm_f)
{
    if (auto const attr = get_sign_ext_attribute(local, proto.ret_type()); attr != llvm::Attribute::None)
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
    if (snippet.open_blocks.size() and std::holds_alternative<typecheck::expr_t::unit_t>(proto.ret_type().value))
    {
        auto builder = llvm::IRBuilder<>(global.llvm_ctx);
        snippet.seal_open_blocks(builder, [] (auto& builder) { builder.CreateRetVoid(); });
    }
}

llvm_func_t gen_func(
    global_context_t& global,
    local_context_t const& local,
    llvm_func_proto_t const& proto,
    typecheck::expr_t::abs_t const& f)
{
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
    llvm_func_proto_t const& proto,
    typecheck::expr_t::abs_t const& f)
{
    auto const llvm_f =
        llvm::Function::Create(
            gen_func_type(global, local, proto),
            llvm::Function::ExternalLinkage,
            name.view(),
            global.llvm_module);
    // add function to the parent context; but generation must happen in the function context
    bool const inserted = local.try_emplace(typecheck::expr_t::var_t{name}, llvm_func_t(llvm_f)).second;
    assert(inserted);
    auto f_ctx = local.extend();
    gen_func_args(f_ctx, proto, llvm_f);
    gen_func_attributes(f_ctx, proto, llvm_f);
    gen_func_body(global, f_ctx, proto, f.body, llvm_f);
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
    return match(
        expr.value,
        [] (typecheck::expr_t::typename_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for a typename");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::bool_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for bool_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::unit_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for unit_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::i8_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for i8_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::i16_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for i16_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::i32_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for i32_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::i64_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for i64_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::u8_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for u8_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::u16_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for u16_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::u32_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for u32_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::u64_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for u64_t");
            __builtin_unreachable();
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
            auto const& type = std::get<typecheck::expr_t>(expr.properties.sort.get());
            auto const llvm_type = cast<llvm::IntegerType>(gen_type(global, local, type));
            assert(llvm_type);
            return llvm::ConstantInt::get(llvm_type, number, 10);
        },
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
        [&] (typecheck::expr_t::var_t const& var) -> llvm::Value*
        {
            auto const val = local[var];
            assert(val and "unknown variable");
            return match(
                *val,
                [] (llvm::Value* const p) { return p; },
                [] (llvm_func_t const& c) { return c.func; },
                [] (typecheck::type_def_t const&) -> llvm::Value*
                {
                    assert(false and "found a typedef but was expecting a value");
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
        [&] (typecheck::expr_t::pi_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for a pi-type");
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
                auto const val = local[var];
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
                    });
            },
            [&] (typecheck::expr_t::abs_t const& abs) -> llvm_func_t
            {
                auto const type = std::get_if<typecheck::expr_t>(&app.func.get().properties.sort.get());
                assert(type and "function type must not be a kind");
                auto const pi = std::get_if<typecheck::expr_t::pi_t>(&type->value);
                assert(pi and "function type must be pi");
                auto proto = llvm_func_proto_t::from_pi(*pi);
                assert(proto and "can only invoke a 1st order function type");
                return gen_func(global, local, *proto, abs);
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
        std::ranges::all_of(
            args,
            [] (typecheck::expr_t const& x)
            {
                return match(
                    x.properties.sort.get(),
                    [] (typecheck::expr_t const& type) { return is_first_order_type(type); },
                    [] (typecheck::kind_t) { return false; });
            })
        and "can only generate function call for 1st order applications");
    auto const call =
        builder.CreateCall(
            callee.type,
            callee.func,
            fmap(args, [&] (typecheck::expr_t const& arg)
            {
                return gen_val(global, local, builder, arg);
            }));
    for (auto const i: std::views::iota(0ul, args.size()))
    {
        auto const& arg_type = std::get<typecheck::expr_t>(args[i].properties.sort.get());
        if (auto const attr = get_sign_ext_attribute(local, arg_type); attr != llvm::Attribute::None)
            call->addParamAttr(i, attr);
    }
    return call;
}

}
