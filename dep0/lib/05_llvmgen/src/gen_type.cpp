#include "private/gen_type.hpp"

#include "private/gen_alloca.hpp"
#include "private/gen_array.hpp"

#include "dep0/match.hpp"

#include <vector>

namespace dep0::llvmgen {

llvm::FunctionType* gen_func_type(
    global_context_t& global,
    local_context_t const& local,
    llvm_func_proto_t const& proto)
{
    bool constexpr is_var_arg = false;
    std::vector<llvm::Type*> arg_types; // might need to contain a return argument
    auto const ret_type =
        match(
            needs_alloca(proto.ret_type()),
            [&] (needs_alloca_result::no_t)
            {
                return gen_type(global, local, proto.ret_type());
            },
            [&] (needs_alloca_result::array_t const& array)
            {
                arg_types.push_back(gen_type(global, local, array.properties.element_type)->getPointerTo());
                return llvm::Type::getVoidTy(global.llvm_ctx);
            });
    arg_types.reserve(arg_types.size() + proto.args().size());
    for (typecheck::func_arg_t const& arg: proto.args())
        arg_types.push_back(gen_type(global, local, arg.type));
    return llvm::FunctionType::get(ret_type, std::move(arg_types), is_var_arg);
}

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
        [&] (typecheck::expr_t::boolean_expr_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a boolean expression");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::relation_expr_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a relation expression");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::arith_expr_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for an arithmetic expression");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::var_t const&) -> llvm::Type*
        {
            assert(false and "found a value but was expecting a type");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::global_t const& g) -> llvm::Type*
        {
            auto const val = global[g];
            assert(val and "unknown type");
            return match(
                *val,
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

} // namespace dep0::llvmgen
