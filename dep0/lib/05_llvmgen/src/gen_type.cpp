#include "private/gen_type.hpp"

#include "private/gen_alloca.hpp"
#include "private/gen_array.hpp"

#include "dep0/match.hpp"

#include <vector>

namespace dep0::llvmgen {

llvm::FunctionType* gen_func_type(global_ctx_t& global, llvm_func_proto_t const& proto)
{
    bool constexpr is_var_arg = false;
    std::vector<llvm::Type*> arg_types; // might need to contain a return argument
    auto const ret_type =
        match(
            needs_alloca(proto.ret_type()),
            [&] (needs_alloca_result::no_t)
            {
                return gen_type(global, proto.ret_type());
            },
            [&] (needs_alloca_result::array_t const& array)
            {
                arg_types.push_back(gen_type(global, array.properties.element_type)->getPointerTo());
                return llvm::Type::getVoidTy(global.llvm_ctx);
            });
    arg_types.reserve(arg_types.size() + proto.runtime_args().size());
    for (typecheck::func_arg_t const& arg: proto.runtime_args())
        arg_types.push_back(gen_type(global, arg.type));
    return llvm::FunctionType::get(ret_type, std::move(arg_types), is_var_arg);
}

llvm::IntegerType* gen_type(global_ctx_t& global, ast::width_t const width)
{
    switch (width)
    {
    case ast::width_t::_8: return llvm::Type::getInt8Ty(global.llvm_ctx);
    case ast::width_t::_16: return llvm::Type::getInt16Ty(global.llvm_ctx);
    case ast::width_t::_32: return llvm::Type::getInt32Ty(global.llvm_ctx);
    case ast::width_t::_64: return llvm::Type::getInt64Ty(global.llvm_ctx);
    default: __builtin_unreachable();
    }
}

llvm::Type* gen_type(global_ctx_t& global, typecheck::expr_t const& x)
{
    return match(
        x.value,
        [] (typecheck::expr_t::typename_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for typename");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::true_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for true_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::auto_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for auto expression");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::bool_t const&) -> llvm::Type* { return llvm::Type::getInt1Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::unit_t const&) -> llvm::Type*
        {
            return llvm::StructType::get(global.llvm_ctx);
        },
        [&] (typecheck::expr_t::i8_t const&) -> llvm::Type* { return llvm::Type::getInt8Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::i16_t const&) -> llvm::Type* { return llvm::Type::getInt16Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::i32_t const&) -> llvm::Type* { return llvm::Type::getInt32Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::i64_t const&) -> llvm::Type* { return llvm::Type::getInt64Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::u8_t const&) -> llvm::Type* { return llvm::Type::getInt8Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::u16_t const&) -> llvm::Type* { return llvm::Type::getInt16Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::u32_t const&) -> llvm::Type* { return llvm::Type::getInt32Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::u64_t const&) -> llvm::Type* { return llvm::Type::getInt64Ty(global.llvm_ctx); },
        [] (typecheck::expr_t::boolean_constant_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a boolean constant");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::numeric_constant_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a numeric constant");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::boolean_expr_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a boolean expression");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::relation_expr_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a relation expression");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::arith_expr_t const&) -> llvm::Type*
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
                            return gen_type(global, integer.width);
                        });
                });
        },
        [&] (typecheck::expr_t::app_t const& app) -> llvm::Type*
        {
            return match(
                app.func.get().value,
                [&] (typecheck::expr_t::true_t const&) -> llvm::Type*
                {
                    return llvm::StructType::get(global.llvm_ctx);
                },
                [&] (typecheck::expr_t::array_t const&) -> llvm::Type*
                {
                    auto const properties = get_array_properties(x);
                    return gen_type(global, properties.element_type)->getPointerTo();
                },
                [] (auto const&) -> llvm::Type*
                {
                    assert(false and "cannot generate a type for application of anything except true_t and array_t");
                    __builtin_unreachable();
                });
        },
        [] (typecheck::expr_t::abs_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a function abstraction");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::pi_t const& x) -> llvm::Type*
        {
            auto proto = llvm_func_proto_t::from_pi(x);
            assert(proto and "can only generate an llvm type for 1st order function types");
            return gen_func_type(global, *proto)->getPointerTo();
        },
        [] (typecheck::expr_t::array_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for an array_t expression");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::init_list_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for an initializer list");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::subscript_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for subscript expression");
            __builtin_unreachable();
        });
}

} // namespace dep0::llvmgen
