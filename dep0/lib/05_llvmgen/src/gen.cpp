#include "dep0/llvmgen/gen.hpp"

#include "dep0/fmap.hpp"
#include "dep0/match.hpp"
#include "dep0/scope_map.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>

#include <boost/range/adaptor/indexed.hpp>

#include <algorithm>
#include <optional>
#include <ranges>

namespace dep0::llvmgen {

static bool has_unit_type(typecheck::expr_t const& expr)
{
    auto const type = std::get_if<typecheck::expr_t>(&expr.properties.sort.get());
    return type and std::holds_alternative<typecheck::expr_t::unit_t>(type->value);
}

struct array_properties_t
{
    typecheck::expr_t const& element_type;
    std::vector<typecheck::expr_t const*> dimensions;
};

static bool is_array(typecheck::expr_t const& type)
{
    auto const app = std::get_if<typecheck::expr_t::app_t>(&type.value);
    return app and std::holds_alternative<typecheck::expr_t::array_t>(app->func.get().value);
}

static std::optional<array_properties_t> get_properties_if_array(typecheck::expr_t const& type)
{
    auto curr = get_if_app_of_array(type);
    if (not curr)
        return std::nullopt;
    std::vector<typecheck::expr_t const*> dimensions;
    dimensions.push_back(&curr->args[1ul]);
    while (auto const next = get_if_app_of_array(curr->args[0ul]))
    {
        dimensions.push_back(&next->args[1ul]);
        curr = next;
    }
    return array_properties_t{curr->args[0ul], std::move(dimensions)};
}

static array_properties_t get_array_properties(typecheck::expr_t const& type)
{
    auto properties = get_properties_if_array(type);
    assert(properties.has_value() and "type must be an array");
    return std::move(*properties);
}

namespace needs_alloca_result
{
    struct no_t{};
    struct array_t
    {
        typecheck::expr_t const& type;
        typecheck::expr_t const& size;
    };
}
using needs_alloca_result_t =
    std::variant<
        needs_alloca_result::no_t,
        needs_alloca_result::array_t>;

static needs_alloca_result_t needs_alloca(typecheck::expr_t const& type)
{
    if (auto const app = get_if_app_of_array(type))
        return needs_alloca_result::array_t{app->args[0ul], app->args[1ul]};
    return needs_alloca_result::no_t{};
}

static bool is_alloca_needed(typecheck::expr_t const& type)
{
    return not std::holds_alternative<needs_alloca_result::no_t>(needs_alloca(type));
}

/**
 * Like `llvm::FunctionCallee`, it represents a callable function together with its type.
 * But unlike it, this allows access to the two fields via const-ref.
 * If upstream fixes their API, we can remove this and use theirs instead.
 */
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

// Proof that some `pi_t` or `abs_t` were a 1st order function type, suitable to generate LLVM functions.
class llvm_func_proto_t
{
    std::vector<typecheck::func_arg_t> m_args;
    typecheck::expr_t m_ret_type;

    llvm_func_proto_t(typecheck::expr_t::pi_t const& x)  : m_args(x.args), m_ret_type(x.ret_type.get()) { }
    llvm_func_proto_t(typecheck::expr_t::abs_t const& x) : m_args(x.args), m_ret_type(x.ret_type.get()) { }
    llvm_func_proto_t(llvm_func_proto_t const&) = delete; // prevent accidental copies
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

static llvm::Value* gen_func(
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
    llvm::Function*,
    llvm::Value* inlined_result);
static void gen_stmt(
    global_context_t&,
    local_context_t const&,
    snippet_t&,
    llvm::IRBuilder<>&,
    typecheck::stmt_t const&,
    llvm::Function*,
    llvm::Value* inlined_result);
static llvm::Type* gen_type(global_context_t&, local_context_t const&, typecheck::expr_t const&);
static llvm::Instruction* gen_alloca(
    global_context_t&,
    local_context_t const&,
    llvm::IRBuilder<>&,
    typecheck::expr_t const&,
    llvm::Value* const size);
static llvm::Instruction* gen_alloca_if_needed(
    global_context_t&,
    local_context_t const&,
    llvm::IRBuilder<>&,
    typecheck::expr_t const&);
static llvm::Value* gen_array_total_size(
    global_context_t&,
    local_context_t const&,
    llvm::IRBuilder<>&,
    array_properties_t const&);
static llvm::Value* gen_stride_size_if_needed(
    global_context_t&,
    local_context_t const&,
    llvm::IRBuilder<>&,
    array_properties_t const&);
static llvm::Value* gen_val(
    global_context_t&,
    local_context_t const&,
    llvm::IRBuilder<>&,
    typecheck::expr_t const&,
    llvm::Value* dest);
static llvm::Value* gen_func_call(
    global_context_t&,
    local_context_t const&,
    llvm::IRBuilder<>&,
    typecheck::expr_t::app_t const&,
    llvm::Value* dest);
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

/**
 * Move the given instruction to the entry block of the given function.
 * If the instruction is already in its entry block, this function does nothing.
 *
 * @param i The instruction to move.
 * @param f The function containing the entry block where the instruction is going to be moved.
 */
static void move_to_entry_block(llvm::Instruction* const i, llvm::Function* const f)
{
    auto& bb = f->getEntryBlock();
    if (&bb == i->getParent())
        // already in entry block
        return;
    if (auto const t = f->getEntryBlock().getTerminator())
        i->moveBefore(t);
    else
        i->moveAfter(&bb.back());
}

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
        [] (typecheck::expr_t::app_t const& x)
        {
            // `array_t(type, size)` is a 1st order type if `type` is of 1st order,
            // regardless of how complex the size expression is;
            // whatever the size, it will always reduce to just a pointer anyway;
            return std::holds_alternative<typecheck::expr_t::array_t>(x.func.get().value)
                and is_first_order_type(x.args[0ul]);
        },
        [] (typecheck::expr_t::abs_t const&) { return false; },
        [] (typecheck::expr_t::pi_t const& t) { return is_first_order_function_type(t.args, t.ret_type.get()); },
        [] (typecheck::expr_t::array_t const&) { return false; }, // `array_t` on its own is a term, not a type
        [] (typecheck::expr_t::init_list_t const&) { return false; },
        [] (typecheck::expr_t::subscript_t const&) { return false; });
}

expected<unique_ref<llvm::Module>>
    gen(llvm::LLVMContext& llvm_ctx, std::string_view const name, typecheck::module_t const& m, verify_t verify)
    noexcept
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
    return verify == verify_t::yes and llvm::verifyModule(llvm_module.get(), &ostream) // yes true means false...
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
        [&] (typecheck::expr_t::unit_t const&) -> llvm::Type*
        {
            // This is worth an explanation, as there were some thoughts put behind this decision.
            // In C/C++ `void` is flawed:
            //  1. when used as return type for a function, it behaves as the unit type;
            //  2. otherwise it behaves as the bottom type (eg function argument or member-field/array-element type).
            // The LLVM IR also has a type `void`, with this same semantics/flaws.
            // So one might think that mapping `unit_t` to LLVM `void` is sensible, for (1);
            // but we would then have to deal with (2).
            // When you try to deal with (2), you open up a Pandora's box;
            // for example, what about arguments of type `unit_t`?
            // We could decide to add "clever" logic to skip them,
            // after all clang seems to do that for empty structs in C++;
            // that would be feasible, but also complicated (and fiddly more than "clever").
            // What about arrays? Do they have 0 size?
            // Perhaps you can do so, but LLVM IR doesn't allow `void*` anyway.
            // A different alternative is to just make `unit_t` 1 byte, always.
            // This decision massively simplifies the logic of emitting LLVM IR,
            // but there is an obvious drawback:
            // a function with return type `unit_t` will now waste a register.
            // Whilst this would be problematic for an efficient executable program,
            // there are 2 important considersations:
            //  1. first of all, the optimizer might realize that the register is not really being used
            //     and could decide to use it for something else;
            //     also, someone interested in performace should maybe consider making their function bodies
            //     visible for inlining, a lot of computations might get performed at compile-time via beta-reduction;
            //  2. traditionally, return type `void` suggests that this is a function whose only purpose is
            //     to have side-effects; but even the most effectful function, say `memset()`, would be better if
            //     it returned the pointer to the end of the region.
            // So, overall, we should regard the idea of making `unit_t` an LLVM type with 0 size as
            // a potential optimization opportunity, rather than a fundamental fact of emitting LLVM IR.
            // So, for now, we prefer to make the codegen logic simpler and deal with this optimization later.
            // The only time we use LLVM `void` is when a function returns an object via a StructRet argument.
            return llvm::Type::getInt8Ty(global.llvm_ctx);
        },
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
            auto const properties = get_array_properties(x);
            return gen_type(global, local, properties.element_type)->getPointerTo();
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
        },
        [&] (typecheck::expr_t::array_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for an array_t expression");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::init_list_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for an initializer list");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::subscript_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for subscript expression");
            __builtin_unreachable();
        });
}

llvm::Attribute::AttrKind get_sign_ext_attribute(local_context_t const& local, typecheck::expr_t const& type)
{
    return match(
        type.value,
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
        [] (typecheck::expr_t::pi_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::array_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::init_list_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::subscript_t const&) { return llvm::Attribute::None; });
}

llvm::FunctionType* gen_func_type(
    global_context_t& global,
    local_context_t const& local,
    llvm_func_proto_t const& proto)
{
    bool constexpr is_var_arg = false;
    auto ret_type = gen_type(global, local, proto.ret_type());
    std::vector<llvm::Type*> arg_types;
    match(
        needs_alloca(proto.ret_type()),
        [] (needs_alloca_result::no_t) {},
        [&] (needs_alloca_result::array_t const& array)
        {
             // For arrays `gen_type` already returns a pointer,
             // so no need to do `ret_type->getPointerTo()`.
            arg_types.push_back(ret_type);
            ret_type = llvm::Type::getVoidTy(global.llvm_ctx);
        });
    arg_types.reserve(arg_types.size() + proto.args().size());
    for (typecheck::func_arg_t const& arg: proto.args())
        arg_types.push_back(gen_type(global, local, arg.type));
    return llvm::FunctionType::get(ret_type, std::move(arg_types), is_var_arg);
}

void gen_func_args(local_context_t& local, llvm_func_proto_t const& proto, llvm::Function* const llvm_f)
{
    auto llvm_arg_it = llvm_f->arg_begin();
    if (is_alloca_needed(proto.ret_type()))
    {
        assert(llvm_f->arg_size() == proto.args().size() + 1ul and "function with sret must have 1 more argument");
        llvm_arg_it->addAttr(llvm::Attribute::StructRet);
        llvm_arg_it->addAttr(llvm::Attribute::NonNull);
        ++llvm_arg_it;
    }
    else
        assert(llvm_f->arg_size() == proto.args().size() and "function and prototype must have same arguments");
    for (auto const& arg: proto.args())
    {
        auto const& llvm_arg = llvm_arg_it++;
        if (auto const attr = get_sign_ext_attribute(local, arg.type); attr != llvm::Attribute::None)
            llvm_arg->addAttr(attr);
        if (is_alloca_needed(arg.type))
            llvm_arg->addAttr(llvm::Attribute::NonNull);
        if (arg.var)
        {
            if (arg.var->idx == 0ul)
                llvm_arg->setName(arg.var->name.view());
            bool inserted = false;
            if (std::holds_alternative<typecheck::expr_t::pi_t>(arg.type.value))
            {
                assert(llvm_arg->getType()->isPointerTy());
                auto const function_type = cast<llvm::FunctionType>(llvm_arg->getType()->getPointerElementType());
                assert(function_type);
                inserted = local.try_emplace(*arg.var, llvm_func_t(function_type, llvm_arg)).second;
            }
            else
                inserted = local.try_emplace(*arg.var, llvm_arg).second;
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
    auto snippet = gen_body(global, local, body, "entry", llvm_f, nullptr);
    if (snippet.open_blocks.size() and std::holds_alternative<typecheck::expr_t::unit_t>(proto.ret_type().value))
    {
        auto builder = llvm::IRBuilder<>(global.llvm_ctx);
        // Having open blocks means that the function has no return statement,
        // this implies its return type is `unit_t`, so just return `i18 0`.
        snippet.seal_open_blocks(builder, [zero=builder.getInt8(0)] (auto& builder) { builder.CreateRet(zero); });
    }
}

llvm::Value* gen_func(
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
    return llvm_f;
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
    llvm::Function* const llvm_f,
    llvm::Value* const inlined_result)
{
    snippet_t snippet;
    auto builder = llvm::IRBuilder<>(global.llvm_ctx);
    builder.SetInsertPoint(snippet.entry_block = llvm::BasicBlock::Create(global.llvm_ctx, entry_block_name, llvm_f));
    auto it = body.stmts.begin();
    if (it == body.stmts.end())
        snippet.open_blocks.push_back(snippet.entry_block);
    else
    {
        gen_stmt(global, local, snippet, builder, *it, llvm_f, inlined_result);
        while (++it != body.stmts.end())
        {
            if (snippet.open_blocks.size())
            {
                auto next = llvm::BasicBlock::Create(global.llvm_ctx, "next", llvm_f);
                snippet.seal_open_blocks(builder, [next] (auto& builder) { builder.CreateBr(next); });
                builder.SetInsertPoint(next);
            }
            gen_stmt(global, local, snippet, builder, *it, llvm_f, inlined_result);
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
    llvm::Function* const llvm_f,
    llvm::Value* const inlined_result)
{
    match(
        stmt.value,
        [&] (typecheck::expr_t::app_t const& x)
        {
            gen_func_call(global, local, builder, x, nullptr);
        },
        [&] (typecheck::stmt_t::if_else_t const& x)
        {
            auto const cond = gen_val(global, local, builder, x.cond, nullptr);
            auto const then = gen_body(global, local, x.true_branch, "then", llvm_f, inlined_result);
            std::ranges::copy(then.open_blocks, std::back_inserter(snipppet.open_blocks));
            if (x.false_branch)
            {
                auto const else_ = gen_body(global, local, *x.false_branch, "else", llvm_f, inlined_result);
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
            if (not x.expr)
            {
                // only unit_t can be missing a return expr
                builder.CreateRet(builder.getInt8(0));
                return;
            }
            if (inlined_result)
            {
                gen_val(global, local, builder, *x.expr, inlined_result);
                snipppet.open_blocks.push_back(builder.GetInsertBlock());
                return;
            }
            auto const arg0 = llvm_f->arg_empty() ? nullptr : llvm_f->getArg(0ul);
            auto const dest = arg0 and arg0->hasAttribute(llvm::Attribute::StructRet) ? arg0 : nullptr;
            // always generate a value, even for expressions of type unit_t,
            // because it might be a function call with side effects;
            // and, if it is, just return 0, without complicating the CFG
            auto const ret_val = gen_val(global, local, builder, *x.expr, dest);
            if (dest)
                builder.CreateRetVoid();
            else if (has_unit_type(*x.expr))
                builder.CreateRet(builder.getInt8(0));
            else
                builder.CreateRet(ret_val);
        });
}

llvm::Instruction* gen_alloca(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t const& type,
    llvm::Value* const size)
{
    return builder.CreateAlloca(gen_type(global, local, type), size);
}

llvm::Instruction* gen_alloca_if_needed(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t const& type)
{
    return match(
        needs_alloca(type),
        [] (needs_alloca_result::no_t) -> llvm::Instruction* { return nullptr; },
        [&] (needs_alloca_result::array_t const& array) -> llvm::Instruction*
        {
            auto const size = gen_val(global, local, builder, array.size, nullptr);
            return gen_alloca(global, local, builder, array.type, size);
        });
}

llvm::Value* gen_array_total_size(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    array_properties_t const& properties)
{
    assert(properties.dimensions.size() > 0ul);
    return std::accumulate(
        std::next(properties.dimensions.begin()), properties.dimensions.end(),
        gen_val(global, local, builder, *properties.dimensions[0ul], nullptr),
        [&] (llvm::Value* const acc, typecheck::expr_t const* size)
        {
            return builder.CreateMul(acc, gen_val(global, local, builder, *size, nullptr));
        });
}

llvm::Value* gen_stride_size_if_needed(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    array_properties_t const& properties)
{
    return
        properties.dimensions.size() > 1ul
        ? std::accumulate(
            std::next(properties.dimensions.begin(), 2ul), properties.dimensions.end(),
            gen_val(global, local, builder, *properties.dimensions[1ul], nullptr),
            [&] (llvm::Value* const acc, typecheck::expr_t const* size)
            {
                return builder.CreateMul(acc, gen_val(global, local, builder, *size, nullptr));
            })
        : nullptr;
}

llvm::Value* gen_val(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t const& expr,
    llvm::Value* const dest)
{
    auto const storeOrReturn = [&] (llvm::Value* const value)
    {
        if (not dest)
            return value;
        if (auto const pty = get_properties_if_array(std::get<typecheck::expr_t>(expr.properties.sort.get())))
        {
            assert(dest->getType()->isPointerTy() and "memcpy destination must be a pointer");
            auto const& data_layout = global.llvm_module.getDataLayout();
            auto const ptr_element_type = dest->getType()->getPointerElementType();
            auto const align = data_layout.getPrefTypeAlign(ptr_element_type);
            auto const bytes = data_layout.getTypeAllocSize(ptr_element_type);
            auto const size = gen_array_total_size(global, local, builder, *pty);
            auto const total_bytes = builder.CreateMul(size, builder.getInt64(bytes.getFixedSize()));
            builder.CreateMemCpy(dest, align, value, align, total_bytes);
        }
        else
            builder.CreateStore(value, dest);
        return dest;
    };
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
            return storeOrReturn(llvm::ConstantInt::getBool(global.llvm_ctx, x.value));
        },
        [&] (typecheck::expr_t::numeric_constant_t const& x) -> llvm::Value*
        {
            // TODO currently beta-delta normalization operates on the templated AST,
            // so it does not reduce inside `properties.sort`, which means we might fail to generate a type;
            // we should in fact check all usages of `properties.sort`
            auto const& type = std::get<typecheck::expr_t>(expr.properties.sort.get());
            auto const llvm_type = cast<llvm::IntegerType>(gen_type(global, local, type));
            assert(llvm_type);
            return storeOrReturn(llvm::ConstantInt::get(llvm_type, x.value.str(), 10));
        },
        [&] (typecheck::expr_t::arith_expr_t const& x) -> llvm::Value*
        {
            return storeOrReturn(match(
                x.value,
                [&] (typecheck::expr_t::arith_expr_t::plus_t const& x) -> llvm::Value*
                {
                    // TODO add NUW/NSW but probably depends on decision whether typechecking `plus_t` requires proofs?
                    // use temporaries to make sure that LHS comes before RHS in the emitted IR
                    auto const lhs = gen_val(global, local, builder, x.lhs.get(), nullptr);
                    auto const rhs = gen_val(global, local, builder, x.rhs.get(), nullptr);
                    return builder.CreateAdd(lhs, rhs);
                }));
        },
        [&] (typecheck::expr_t::var_t const& var) -> llvm::Value*
        {
            auto const val = local[var];
            assert(val and "unknown variable");
            return storeOrReturn(match(
                *val,
                [] (llvm::Value* const p) { return p; },
                [] (llvm_func_t const& c) { return c.func; },
                [] (typecheck::type_def_t const&) -> llvm::Value*
                {
                    assert(false and "found a typedef but was expecting a value");
                    __builtin_unreachable();
                }));
        },
        [&] (typecheck::expr_t::app_t const& x) -> llvm::Value*
        {
            if (auto const abs = std::get_if<typecheck::expr_t::abs_t>(&x.func.get().value))
                if (x.args.empty())
                {
                    // found an immediately-invoked-lambda: inline it!
                    auto const current_func = builder.GetInsertBlock()->getParent();
                    auto const gen_inlined_body = [&] (llvm::Value* const inlined_result)
                    {
                        auto snippet = gen_body(global, local, abs->body, "inlined", current_func, inlined_result);
                        builder.CreateBr(snippet.entry_block);
                        bool const merged = llvm::MergeBlockIntoPredecessor(snippet.entry_block);
                        assert(merged and "llvm could not merge inlined entry block");
                        auto const next_block = llvm::BasicBlock::Create(global.llvm_ctx, "cont", current_func);
                        snippet.seal_open_blocks(
                            builder,
                            [&] (llvm::IRBuilder<>& builder)
                            {
                                builder.CreateBr(next_block);
                            });
                        builder.SetInsertPoint(next_block);
                    };
                    auto const dest2 = dest ? dest : gen_alloca_if_needed(global, local, builder, abs->ret_type.get());
                    // Note, if we generated an alloca here,
                    // there is no need to move it to the entry block,
                    // because mem2reg will not be able to optimize it.
                    if (dest2)
                    {
                        gen_inlined_body(dest2);
                        return dest2;
                    }
                    else
                    {
                        auto const inlined_type = gen_type(global, local, abs->ret_type.get());
                        auto const inlined_result = builder.CreateAlloca(inlined_type, builder.getInt32(1));
                        move_to_entry_block(inlined_result, current_func);
                        gen_inlined_body(inlined_result);
                        return builder.CreateLoad(inlined_type, inlined_result);
                    }
                }
            return gen_func_call(global, local, builder, x, dest);
        },
        [&] (typecheck::expr_t::abs_t const& abs) -> llvm::Value*
        {
            auto const proto = llvm_func_proto_t::from_abs(abs);
            assert(proto and "can only invoke a 1st order function type");
            return storeOrReturn(gen_func(global, local, *proto, abs));
        },
        [&] (typecheck::expr_t::pi_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for a pi-type");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::array_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for an array_t expression");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::init_list_t const& x) -> llvm::Value*
        {
            auto const properties = get_array_properties(std::get<typecheck::expr_t>(expr.properties.sort.get()));
            auto const dest2 = dest ? dest : [&]
            {
                auto const total_size = gen_array_total_size(global, local, builder, properties);
                return gen_alloca(global, local, builder, properties.element_type, total_size);
            }();
            auto const type = dest2->getType()->getPointerElementType();
            auto const stride_size = gen_stride_size_if_needed(global, local, builder, properties);
            auto const int32 = llvm::Type::getInt32Ty(global.llvm_ctx); // TODO we use u64_t to typecheck arrays
            for (auto const i: std::views::iota(0ul, x.values.size()))
            {
                auto const index = llvm::ConstantInt::get(int32, i);
                auto const offset = stride_size ? builder.CreateMul(stride_size, index) : index;
                auto const p = builder.CreateGEP(type, dest2, offset);
                gen_val(global, local, builder, x.values[i], p);
            }
            return dest2;
        },
        [&] (typecheck::expr_t::subscript_t const& subscript) -> llvm::Value*
        {
            auto const& array = subscript.array.get();
            auto const properties = get_array_properties(std::get<typecheck::expr_t>(array.properties.sort.get()));
            auto const stride_size = gen_stride_size_if_needed(global, local, builder, properties);
            auto const type = gen_type(global, local, properties.element_type);
            auto const base = gen_val(global, local, builder, subscript.array.get(), nullptr);
            auto const index = gen_val(global, local, builder, subscript.index.get(), nullptr);
            auto const offset = stride_size ? builder.CreateMul(stride_size, index) : index;
            auto const ptr = builder.CreateGEP(type, base, offset);
            return storeOrReturn(
                is_array(std::get<typecheck::expr_t>(expr.properties.sort.get()))
                    ? ptr
                    : builder.CreateLoad(type, ptr));
        });
}

llvm::Value* gen_func_call(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t::app_t const& app,
    llvm::Value* const dest)
{
    assert(
        std::ranges::all_of(
            app.args,
            [] (typecheck::expr_t const& x)
            {
                return match(
                    x.properties.sort.get(),
                    [] (typecheck::expr_t const& type) { return is_first_order_type(type); },
                    [] (typecheck::kind_t) { return false; });
            })
        and "can only generate function call for 1st order applications");
    auto const proto = [&] () -> llvm_func_proto_t
    {
        auto const func_type = std::get_if<typecheck::expr_t>(&app.func.get().properties.sort.get());
        assert(func_type and "functions must be of sort type");
        auto const pi_type = std::get_if<typecheck::expr_t::pi_t>(&func_type->value);
        assert(pi_type and "functions must be pi-types");
        auto proto = llvm_func_proto_t::from_pi(*pi_type);
        assert(proto and "can only generate a function call for 1st order function type");
        return std::move(proto.value());
    }();
    auto const llvm_func_type = gen_func_type(global, local, proto);
    auto const llvm_func = gen_val(global, local, builder, app.func.get(), nullptr);
    std::vector<llvm::Value*> llvm_args; // modifiable before generating the call
    auto const has_ret_arg = is_alloca_needed(proto.ret_type());
    auto const gen_call = [&, arg_offset = has_ret_arg ? 1ul : 0ul]
    {
        llvm_args.reserve(llvm_args.size() + app.args.size());
        for (typecheck::expr_t const& arg: app.args)
            llvm_args.push_back(gen_val(global, local, builder, arg, nullptr));
        // this could be a direct call to a global function, or an indirect call via a function pointer
        auto const call = builder.CreateCall(llvm_func_type, llvm_func, std::move(llvm_args));
        for (auto const i: std::views::iota(0ul, app.args.size()))
        {
            auto const& arg_type = std::get<typecheck::expr_t>(app.args[i].properties.sort.get());
            if (auto const attr = get_sign_ext_attribute(local, arg_type); attr != llvm::Attribute::None)
                call->addParamAttr(i + arg_offset, attr);
        }
        return call;
    };
    if (has_ret_arg)
    {
        auto const dest2 = dest ? dest : gen_alloca_if_needed(global, local, builder, proto.ret_type());
        assert(dest2 and "function call needs an alloca for the return value but does not have one");
        llvm_args.push_back(dest2);
        gen_call();
        return dest2;
    }
    else if (dest)
    {
        builder.CreateStore(gen_call(), dest);
        return dest;
    }
    else
        return gen_call();
}

} // namespace dep0::llvmgen
