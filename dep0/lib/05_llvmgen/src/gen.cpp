#include "dep0/llvmgen/gen.hpp"

#include "dep0/digit_separator.hpp"
#include "dep0/fmap.hpp"
#include "dep0/match.hpp"
#include "dep0/scope_map.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>

#include <algorithm>
#include <ranges>

namespace dep0::llvmgen {

struct context_t
{
    scope_map<source_text, llvm::Value*> values; // including function objects
    scope_map<source_text, llvm::FunctionType*> fun_types;
    scope_map<source_text, llvm::Type*> types;
    scope_map<source_text, typecheck::type_def_t::integer_t> integer_types;

    context_t() = default;
    context_t(
        scope_map<source_text, llvm::Value*> values,
        scope_map<source_text, llvm::FunctionType*> fun_types,
        scope_map<source_text, llvm::Type*> types,
        scope_map<source_text, typecheck::type_def_t::integer_t> integer_types
    ) : values(std::move(values)),
        fun_types(std::move(fun_types)),
        types(std::move(types)),
        integer_types(std::move(integer_types))
    { }

    context_t extend() const
    {
        return context_t(
            values.extend(),
            fun_types.extend(),
            types.extend(),
            integer_types.extend()
        );
    }
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
};

// all gen functions must be forward-declared here

static llvm::Type* gen_type(context_t const&, llvm::LLVMContext&, typecheck::type_t const&);
static llvm::Type* gen_type(context_t&, llvm::LLVMContext&, typecheck::type_def_t const&);
static llvm::Value* gen_func(context_t&, llvm::Module& , typecheck::func_def_t const&);
static snippet_t gen_body(
    context_t const&,
    llvm::LLVMContext&,
    typecheck::body_t const&,
    std::string_view,
    llvm::Function*);
static void gen_stmt(context_t const&, snippet_t&, llvm::IRBuilder<>&, typecheck::stmt_t const&, llvm::Function*);
static llvm::Value* gen_val(context_t const&, llvm::IRBuilder<>& , typecheck::expr_t const&);
static llvm::CallInst* gen_func_call(context_t const&, llvm::IRBuilder<>&, typecheck::expr_t::app_t const&);

expected<unique_ref<llvm::Module>> gen(
    llvm::LLVMContext& llvm_ctx,
    std::string_view const name,
    typecheck::module_t const& m)
{
    context_t ctx;
    auto llvm_module = make_ref<llvm::Module>(name, llvm_ctx);
    // TODO: allow declarations in any order with 2 pass (both type and function defs)
    for (auto const& t: m.type_defs)
        gen_type(ctx, llvm_ctx, t);
    for (auto const& f: m.func_defs)
        gen_func(ctx, llvm_module.get(), f);

    std::string err;
    llvm::raw_string_ostream ostream(err);
    return llvm::verifyModule(llvm_module.get(), &ostream) // yes true means false...
        ? expected<unique_ref<llvm::Module>>{error_t{err}}
        : expected<unique_ref<llvm::Module>>{std::in_place, std::move(llvm_module)};
}

// all gen functions must be implemented here
llvm::Type* gen_type(context_t const& ctx, llvm::LLVMContext& llvm_ctx, typecheck::type_t const& x)
{
    return match(
        x.value,
        [&] (typecheck::type_t::bool_t const&) -> llvm::Type* { return llvm::Type::getInt1Ty(llvm_ctx); },
        // this may or may not be fine depending on how LLVM defines `void` (i.e. what properties it has)
        // for instance, what does it mean to have an array of `void` types in LLVM?
        [&] (typecheck::type_t::unit_t const&) -> llvm::Type* { return llvm::Type::getVoidTy(llvm_ctx); },
        [&] (typecheck::type_t::i8_t const&) -> llvm::Type* { return llvm::Type::getInt8Ty(llvm_ctx); },
        [&] (typecheck::type_t::i16_t const&) -> llvm::Type* { return llvm::Type::getInt16Ty(llvm_ctx); },
        [&] (typecheck::type_t::i32_t const&) -> llvm::Type* { return llvm::Type::getInt32Ty(llvm_ctx); },
        [&] (typecheck::type_t::i64_t const&) -> llvm::Type* { return llvm::Type::getInt64Ty(llvm_ctx); },
        [&] (typecheck::type_t::u8_t const&) -> llvm::Type* { return llvm::Type::getInt8Ty(llvm_ctx); },
        [&] (typecheck::type_t::u16_t const&) -> llvm::Type* { return llvm::Type::getInt16Ty(llvm_ctx); },
        [&] (typecheck::type_t::u32_t const&) -> llvm::Type* { return llvm::Type::getInt32Ty(llvm_ctx); },
        [&] (typecheck::type_t::u64_t const&) -> llvm::Type* { return llvm::Type::getInt64Ty(llvm_ctx); },
        [&] (typecheck::type_t::name_t const& name) -> llvm::Type*
        {
            auto t = ctx.types[name.name];
            assert(t);
            return *t;
        });
}

llvm::Type* gen_type(context_t& ctx, llvm::LLVMContext& llvm_ctx, typecheck::type_def_t const& x)
{
    return match(
        x.value,
        [&] (typecheck::type_def_t::integer_t const& x) -> llvm::Type*
        {
            auto* const t = [&]
            {
                switch (x.width)
                {
                case ast::width_t::_8: return llvm::Type::getInt8Ty(llvm_ctx);
                case ast::width_t::_16: return llvm::Type::getInt16Ty(llvm_ctx);
                case ast::width_t::_32: return llvm::Type::getInt32Ty(llvm_ctx);
                case ast::width_t::_64: return llvm::Type::getInt64Ty(llvm_ctx);
                default: __builtin_unreachable();
                }
            }();
            if (not std::get<bool>(ctx.integer_types.try_emplace(x.name, x)))
            {
                assert(false);
                return nullptr;
            }
            if (not std::get<bool>(ctx.types.try_emplace(x.name, t)))
            {
                assert(false);
                return nullptr;
            }
            return t;
        });
}

llvm::Attribute::AttrKind get_sign_ext_attribute(context_t const& ctx, typecheck::type_t const& x)
{
    return match(
        x.value,
        [] (typecheck::type_t::bool_t const&) { return llvm::Attribute::None; },
        [] (typecheck::type_t::unit_t const&) { return llvm::Attribute::None;; },
        [] (typecheck::type_t::i8_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::type_t::i16_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::type_t::i32_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::type_t::i64_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::type_t::u8_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::type_t::u16_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::type_t::u32_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::type_t::u64_t const&) { return llvm::Attribute::ZExt; },
        [&] (typecheck::type_t::name_t const& x)
        {
            auto const t = ctx.integer_types[x.name];
            return
                not t ? llvm::Attribute::None :
                t->sign == ast::sign_t::signed_v ? llvm::Attribute::SExt
                : llvm::Attribute::ZExt;
        });
}

static void gen_fun_attributes(context_t const& ctx, llvm::Function* const func, typecheck::func_def_t const& x)
{
    if (auto const attr = get_sign_ext_attribute(ctx, x.value.ret_type); attr != llvm::Attribute::None)
        func->addAttribute(llvm::AttributeList::ReturnIndex, attr);
}

llvm::Value* gen_func(context_t& ctx, llvm::Module& llvm_module, typecheck::func_def_t const& x)
{
    auto const is_typename = [](typecheck::expr_t::abs_t::arg_t const& arg)
    {
        return std::holds_alternative<ast::typename_t>(arg.sort);
    };
    if (std::any_of(x.value.args.begin(), x.value.args.end(), is_typename))
        return nullptr;
    auto* const funtype =
        llvm::FunctionType::get(
            gen_type(ctx, llvm_module.getContext(), x.value.ret_type),
            fmap(x.value.args, [&] (auto const& arg)
            {
                return gen_type(ctx, llvm_module.getContext(), std::get<typecheck::type_t>(arg.type));
            }),
            /*isVarArg*/false);
    auto* const func = llvm::Function::Create(funtype, llvm::Function::ExternalLinkage, x.name.view(), llvm_module);
    // add function type and object to the parent context;
    // but generation must happen in the function context
    if (not std::get<bool>(ctx.fun_types.try_emplace(x.name, funtype)))
    {
        assert(false);
        return nullptr;
    }
    if (not std::get<bool>(ctx.values.try_emplace(x.name, func)))
    {
        assert(false);
        return nullptr;
    }
    auto f_ctx = ctx.extend();
    for (auto const i: std::views::iota(0ul, x.value.args.size()))
    {
        auto* const arg = func->getArg(i);
        arg->setName(x.value.args[i].name.view());
        if (auto const attr = get_sign_ext_attribute(f_ctx, x.value.args[i].type); attr != llvm::Attribute::None)
            arg->addAttr(attr);
        if (not std::get<bool>(f_ctx.values.try_emplace(x.value.args[i].name, arg)))
        {
            assert(false);
            return nullptr;
        }
    }
    gen_fun_attributes(f_ctx, func, x);
    auto snippet = gen_body(f_ctx, llvm_module.getContext(), x.value.body, "entry", func);
    if (snippet.open_blocks.size() and std::holds_alternative<typecheck::type_t::unit_t>(x.value.ret_type.value))
    {
        auto builder = llvm::IRBuilder<>(llvm_module.getContext());
        for (auto* const bb: snippet.open_blocks)
        {
            builder.SetInsertPoint(bb);
            builder.CreateRetVoid();
        }
    }
    return func;
}

snippet_t gen_body(
    context_t const& ctx,
    llvm::LLVMContext& llvm_ctx,
    typecheck::body_t const& x,
    std::string_view const entry_block_name,
    llvm::Function* const func)
{
    snippet_t snippet;
    auto builder = llvm::IRBuilder<>(llvm_ctx);
    builder.SetInsertPoint(snippet.entry_block = llvm::BasicBlock::Create(llvm_ctx, entry_block_name, func));
    auto it = x.stmts.begin();
    if (it == x.stmts.end())
        snippet.open_blocks.push_back(snippet.entry_block);
    else
    {
        gen_stmt(ctx, snippet, builder, *it, func);
        while (++it != x.stmts.end())
        {
            if (snippet.open_blocks.size())
            {
                auto next = llvm::BasicBlock::Create(llvm_ctx, "next", func);
                for (auto& bb: snippet.open_blocks)
                {
                    builder.SetInsertPoint(bb);
                    builder.CreateBr(next);
                }
                snippet.open_blocks.clear();
                builder.SetInsertPoint(next);
            }
            gen_stmt(ctx, snippet, builder, *it, func);
        }
    }
    return snippet;
}

void gen_stmt(
    context_t const& ctx,
    snippet_t& snipppet,
    llvm::IRBuilder<>& builder,
    typecheck::stmt_t const& x,
    llvm::Function* const func)
{
    match(
        x.value,
        [&] (typecheck::expr_t::app_t const& x)
        {
            gen_func_call(ctx, builder, x);
        },
        [&] (typecheck::stmt_t::if_else_t const& x)
        {
            auto* cond = gen_val(ctx, builder, x.cond);
            auto then = gen_body(ctx, builder.getContext(), x.true_branch, "then", func);
            std::ranges::copy(then.open_blocks, std::back_inserter(snipppet.open_blocks));
            if (x.false_branch)
            {
                auto else_ = gen_body(ctx, builder.getContext(), *x.false_branch, "else", func);
                std::ranges::copy(else_.open_blocks, std::back_inserter(snipppet.open_blocks));
                builder.CreateCondBr(cond, then.entry_block, else_.entry_block);
            }
            else
            {
                auto else_ = llvm::BasicBlock::Create(builder.getContext(), "else", func);
                builder.CreateCondBr(cond, then.entry_block, else_);
                snipppet.open_blocks.push_back(else_);
            }
        },
        [&] (typecheck::stmt_t::return_t const& x)
        {
            if (x.expr)
                builder.CreateRet(gen_val(ctx, builder, *x.expr));
            else
                builder.CreateRetVoid();
        });
}

llvm::Value* gen_val(context_t const& ctx, llvm::IRBuilder<>& builder, typecheck::expr_t const& expr)
{
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
                        gen_val(ctx, builder, x.lhs.get()),
                        gen_val(ctx, builder, x.rhs.get()));
                });
        },
        [&] (typecheck::expr_t::boolean_constant_t const& x) -> llvm::Value*
        {
            return llvm::ConstantInt::getBool(builder.getContext(), x.value == "true");
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
            auto const llvm_type = cast<llvm::IntegerType>(gen_type(ctx, builder.getContext(), expr.properties.type));
            return llvm::ConstantInt::get(llvm_type, number, 10);
        },
        [&] (typecheck::expr_t::var_t const& x) -> llvm::Value*
        {
            auto const v = ctx.values[x.name];
            assert(v);
            return *v;
        },
        [&] (typecheck::expr_t::app_t const& x) -> llvm::Value*
        {
            return gen_func_call(ctx, builder, x);
        },
        [&] (typecheck::expr_t::abs_t const& x) -> llvm::Value*
        {
            assert(false and "Generation of abstraction expression not yet implemented");
            return nullptr;
        },
        [&] (typecheck::expr_t::type_t const& x) -> llvm::Value*
        {
            assert(false and "Generation of type expression not yet implemented");
            return nullptr;
        });
}

static llvm::CallInst* gen_func_call(context_t const& ctx, llvm::IRBuilder<>& builder, typecheck::expr_t::app_t const& f)
{
    auto const ty = ctx.fun_types[f.name];
    auto const fn = ctx.values[f.name];
    assert(ty);
    assert(fn);
    return builder.CreateCall(*ty, *fn, fmap(f.args, [&] (auto const& arg) { return gen_val(ctx, builder, arg); }));
}

}
