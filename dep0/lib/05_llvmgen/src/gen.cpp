#include "dep0/llvmgen/gen.hpp"

#include "private/mangler.hpp"

#include "dep0/typecheck/substitute.hpp"

#include "dep0/ast/traverse.hpp"

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

struct llvm_func_t
{
    llvm::FunctionType* type;
    llvm::Value* func;
};

struct context_t
{
    scope_map<source_text, llvm::Value*> values; // but not function objects
    scope_map<source_text, llvm_func_t> functions;
    scope_map<source_text, llvm::Type*> types;
    scope_map<source_text, typecheck::type_def_t::integer_t> integer_types;
    scope_map<std::string, llvm_func_t> specialized_functions; // key is mangled name

    context_t() = default;
    context_t(
        scope_map<source_text, llvm::Value*> values,
        scope_map<source_text, llvm_func_t> functions,
        scope_map<source_text, llvm::Type*> types,
        scope_map<source_text, typecheck::type_def_t::integer_t> integer_types,
        scope_map<std::string, llvm_func_t> specialized_functions
    ) : values(std::move(values)),
        functions(std::move(functions)),
        types(std::move(types)),
        integer_types(std::move(integer_types)),
        specialized_functions(std::move(specialized_functions))
    { }

    context_t extend() const
    {
        return context_t(
            values.extend(),
            functions.extend(),
            types.extend(),
            integer_types.extend(),
            specialized_functions.extend()
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

struct llvm_func_proto_t
{
    struct arg_t
    {
        source_text name;
        typecheck::type_t type;
    };
    std::vector<arg_t> args;
    typecheck::type_t ret_type;
};

// all gen functions must be forward-declared here

static llvm::Type* gen_type(context_t const&, llvm::LLVMContext&, typecheck::type_t const&);
static llvm::Type* gen_type(context_t&, llvm::LLVMContext&, typecheck::type_def_t const&);
static llvm::Value* gen_func(context_t&, llvm::Module&, source_text const&, typecheck::expr_t::abs_t const&);
static llvm::Value* gen_specialized_func(
    context_t&,
    llvm::Module&,
    std::string mangled_name,
    typecheck::expr_t::abs_t const&,
    std::vector<typecheck::expr_t> const&);

static snippet_t gen_body(
    context_t const&,
    llvm::LLVMContext&,
    typecheck::body_t const&,
    std::string_view,
    llvm::Function*);
static void gen_stmt(context_t const&, snippet_t&, llvm::IRBuilder<>&, typecheck::stmt_t const&, llvm::Function*);
static llvm::Value* gen_val(context_t const&, llvm::IRBuilder<>& , typecheck::expr_t const&);
static llvm::CallInst* gen_func_call(context_t const&, llvm::IRBuilder<>&, typecheck::expr_t::app_t const&);

static bool is_first_order_abstraction(typecheck::expr_t::abs_t const& f)
{
    return std::ranges::all_of(f.args, [] (auto const& x) { return ast::is_type(x.sort); });
}

static bool is_first_order_function_type(typecheck::type_t::arr_t const& x)
{
    return std::ranges::all_of(x.arg_types, [](auto const& t) { return std::holds_alternative<typecheck::type_t>(t); });
}

expected<unique_ref<llvm::Module>> gen(
    llvm::LLVMContext& llvm_ctx,
    std::string_view const name,
    typecheck::module_t const& m)
{
    context_t ctx;
    auto llvm_module = make_ref<llvm::Module>(name, llvm_ctx);
    // TODO: allow declarations in any order with 2 pass (both type and function defs)
    for (auto const& def: m.type_defs)
        gen_type(ctx, llvm_ctx, def);
    for (auto const& def: m.func_defs)
    {
        if (is_first_order_abstraction(def.value))
            gen_func(ctx, llvm_module.get(), def.name, def.value);
        else
        {
            // we can only generate llvm functions for 1st order abstractions;
            // for 2nd order abstractions we generate a specialized function for each 2nd order application
            // (aka monomorphization) but they can be anywhere, so need to scan all function definitions
            // TODO: currently we don't allow 2 pass definitions, so there's no need to restart from the
            // beginning of the module
            ast::traverse(
                m,
                [&] (typecheck::expr_t::app_t const& app)
                {
                    if (app.name != def.name)
                        return;
                    // TODO shall we also specialize for constant expressions (aka constant propagation)?
                    // maybe behind some aggressive optimisation flag?
                    auto mangled_name = [&]
                    {
                        std::vector<typecheck::type_t> type_args;
                        type_args.reserve(app.args.size());
                        for (auto const& arg: app.args)
                            if (auto const p = std::get_if<typecheck::type_t>(&arg.value))
                                type_args.push_back(*p);
                        return make_mangled_name(app.name, type_args);
                    }();
                    if (not ctx.specialized_functions[mangled_name])
                        gen_specialized_func(ctx, llvm_module.get(), std::move(mangled_name), def.value, app.args);
                });
        }
    }

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
        [&] (typecheck::type_t::var_t const& name) -> llvm::Type*
        {
            auto t = ctx.types[name.name];
            assert(t);
            return *t;
        },
        [&] (typecheck::type_t::arr_t const& x) -> llvm::Type*
        {
            assert(is_first_order_function_type(x) and "can only generate an llvm type for 1st order function types");
            bool constexpr is_var_arg = false;
            return llvm::FunctionType::get(
                gen_type(ctx, llvm_ctx, x.ret_type.get()),
                fmap(
                    x.arg_types,
                    [&] (auto const& t)
                    {
                        return gen_type(ctx, llvm_ctx, std::get<typecheck::type_t>(t));
                    }),
                is_var_arg);
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
        [&] (typecheck::type_t::var_t const& x)
        {
            auto const t = ctx.integer_types[x.name];
            return
                not t ? llvm::Attribute::None :
                t->sign == ast::sign_t::signed_v ? llvm::Attribute::SExt
                : llvm::Attribute::ZExt;
        },
        [] (typecheck::type_t::arr_t const&) { return llvm::Attribute::None;; });
}

static llvm::FunctionType* gen_func_type(context_t& ctx, llvm::LLVMContext& llvm_ctx, llvm_func_proto_t const& proto)
{
    bool constexpr is_var_arg = false;
    return llvm::FunctionType::get(
        gen_type(ctx, llvm_ctx, proto.ret_type),
        fmap(proto.args, [&] (auto const& arg) { return gen_type(ctx, llvm_ctx, arg.type); }),
        is_var_arg);
}

static void gen_func_args(context_t& ctx, llvm_func_proto_t const& proto, llvm::Function* const llvm_f)
{
    for (auto const i: std::views::iota(0ul, proto.args.size()))
    {
        auto* const llvm_arg = llvm_f->getArg(i);
        llvm_arg->setName(proto.args[i].name.view());
        if (auto const attr = get_sign_ext_attribute(ctx, proto.args[i].type); attr != llvm::Attribute::None)
            llvm_arg->addAttr(attr);
        if (not std::get<bool>(ctx.values.try_emplace(proto.args[i].name, llvm_arg)))
            assert(false);
    }
}

static void gen_func_attributes(context_t const& ctx, llvm_func_proto_t const& proto, llvm::Function* const llvm_f)
{
    if (auto const attr = get_sign_ext_attribute(ctx, proto.ret_type); attr != llvm::Attribute::None)
        llvm_f->addAttribute(llvm::AttributeList::ReturnIndex, attr);
}

static void gen_func_body(
    context_t const& ctx,
    llvm::LLVMContext& llvm_ctx,
    llvm_func_proto_t const& proto,
    typecheck::body_t const& body,
    llvm::Function* const llvm_f)
{
    auto snippet = gen_body(ctx, llvm_ctx, body, "entry", llvm_f);
    // seal open blocks
    if (snippet.open_blocks.size() and std::holds_alternative<typecheck::type_t::unit_t>(proto.ret_type.value))
    {
        auto builder = llvm::IRBuilder<>(llvm_ctx);
        for (auto* const bb: snippet.open_blocks)
        {
            builder.SetInsertPoint(bb);
            builder.CreateRetVoid();
        }
    }
}

llvm::Value* gen_func(
    context_t& ctx,
    llvm::Module& llvm_module,
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
                    return llvm_func_proto_t::arg_t{arg.name, std::get<typecheck::type_t>(arg.sort)};
                }),
            f.ret_type};
    auto* const func_type = gen_func_type(ctx, llvm_module.getContext(), proto);
    auto* const llvm_f = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, name.view(), llvm_module);
    // add function type and object to the parent context; but generation must happen in the function context
    bool const inserted = ctx.functions.try_emplace(name, func_type, llvm_f).second;
    assert(inserted);
    auto f_ctx = ctx.extend();
    gen_func_args(f_ctx, proto, llvm_f);
    gen_func_attributes(f_ctx, proto, llvm_f);
    gen_func_body(f_ctx, llvm_module.getContext(), proto, f.body, llvm_f);
    return llvm_f;
}

llvm::Value* gen_specialized_func(
    context_t& ctx,
    llvm::Module& llvm_module,
    std::string mangled_name,
    typecheck::expr_t::abs_t const& f,
    std::vector<typecheck::expr_t> const& args)
{
    assert(f.args.size() == args.size());
    llvm_func_proto_t proto{{}, f.ret_type};
    proto.args.reserve(args.size());
    for (auto const& [i, arg]: boost::adaptors::index(args))
        match(
            arg.properties.sort,
            [&] (typecheck::type_t const& t)
            {
                proto.args.emplace_back(f.args[i].name, t);
            },
            [&] (ast::typename_t const&)
            {
                // this is not ideal...substitution was already preformed during typechecking;
                // ideally we would reuse that result but so far I haven't found a way to structure the
                // code in a way that makes sense whilst still giving access to the pre-computed return type
                substitute(proto.ret_type, f.args[i].name, std::get<typecheck::type_t>(arg.value));
            });
    auto* const func_type = gen_func_type(ctx, llvm_module.getContext(), proto);
    // add specialized function to parent context; but generation must happen in the function context
    auto *const llvm_f = llvm::Function::Create(func_type, llvm::Function::PrivateLinkage, mangled_name, llvm_module);
    bool const inserted = ctx.specialized_functions.try_emplace(std::move(mangled_name), func_type, llvm_f).second;
    assert(inserted);
    auto f_ctx = ctx.extend();
    gen_func_args(f_ctx, proto, llvm_f);
    gen_func_attributes(f_ctx, proto, llvm_f);
    gen_func_body(f_ctx, llvm_module.getContext(), proto, f.body, llvm_f);
    return llvm_f;
}

snippet_t gen_body(
    context_t const& ctx,
    llvm::LLVMContext& llvm_ctx,
    typecheck::body_t const& body,
    std::string_view const entry_block_name,
    llvm::Function* const llvm_f)
{
    snippet_t snippet;
    auto builder = llvm::IRBuilder<>(llvm_ctx);
    builder.SetInsertPoint(snippet.entry_block = llvm::BasicBlock::Create(llvm_ctx, entry_block_name, llvm_f));
    auto it = body.stmts.begin();
    if (it == body.stmts.end())
        snippet.open_blocks.push_back(snippet.entry_block);
    else
    {
        gen_stmt(ctx, snippet, builder, *it, llvm_f);
        while (++it != body.stmts.end())
        {
            if (snippet.open_blocks.size())
            {
                auto next = llvm::BasicBlock::Create(llvm_ctx, "next", llvm_f);
                for (auto& bb: snippet.open_blocks)
                {
                    builder.SetInsertPoint(bb);
                    builder.CreateBr(next);
                }
                snippet.open_blocks.clear();
                builder.SetInsertPoint(next);
            }
            gen_stmt(ctx, snippet, builder, *it, llvm_f);
        }
    }
    return snippet;
}

void gen_stmt(
    context_t const& ctx,
    snippet_t& snipppet,
    llvm::IRBuilder<>& builder,
    typecheck::stmt_t const& stmt,
    llvm::Function* const llvm_f)
{
    match(
        stmt.value,
        [&] (typecheck::expr_t::app_t const& x)
        {
            gen_func_call(ctx, builder, x);
        },
        [&] (typecheck::stmt_t::if_else_t const& x)
        {
            auto* cond = gen_val(ctx, builder, x.cond);
            auto then = gen_body(ctx, builder.getContext(), x.true_branch, "then", llvm_f);
            std::ranges::copy(then.open_blocks, std::back_inserter(snipppet.open_blocks));
            if (x.false_branch)
            {
                auto else_ = gen_body(ctx, builder.getContext(), *x.false_branch, "else", llvm_f);
                std::ranges::copy(else_.open_blocks, std::back_inserter(snipppet.open_blocks));
                builder.CreateCondBr(cond, then.entry_block, else_.entry_block);
            }
            else
            {
                auto else_ = llvm::BasicBlock::Create(builder.getContext(), "else", llvm_f);
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
            auto const llvm_type = cast<llvm::IntegerType>(gen_type(ctx, builder.getContext(), *type));
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
        [&] (typecheck::type_t const& x) -> llvm::Value*
        {
            assert(false and "Generation of an llvm value from a type expression is not possible");
            return nullptr;
        });
}

static llvm::CallInst* gen_func_call(context_t const& ctx, llvm::IRBuilder<>& builder, typecheck::expr_t::app_t const& f)
{
    // the llvm function being called could have been generated by either:
    //   - a 1st order abstraction, in which case we should find its type in `functions`
    //   - or a 2nd order abstraction, in which case we should find a specialization for the types it is applied to
    if (auto const x = ctx.functions[f.name])
    {
        auto const& args = fmap(f.args, [&] (auto const& arg) { return gen_val(ctx, builder, arg); });
        return builder.CreateCall(x->type, x->func, args);
    }
    else
    {
        std::vector<typecheck::type_t> type_args;
        std::vector<typecheck::expr_t> expr_args;
        type_args.reserve(f.args.size());
        expr_args.reserve(f.args.size());
        for (auto const& arg: f.args)
            if (auto const p = std::get_if<typecheck::type_t>(&arg.value))
                type_args.push_back(*p);
            else
                expr_args.push_back(arg);
        auto const mangled_name = make_mangled_name(f.name, type_args);
        auto const specialized_func = ctx.specialized_functions[mangled_name];
        assert(specialized_func);
        return builder.CreateCall(
            specialized_func->type,
            specialized_func->func,
            fmap(expr_args, [&] (auto const& arg) { return gen_val(ctx, builder, arg); }));
    }
}

}
