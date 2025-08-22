#
# Copyright Raffaele Rossi 2023 - 2025.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
#
add_subdirectory(test)

add_library(dep0_llvmgen_lib
    # public headers
    include/dep0/llvmgen/gen.hpp
    # private headers
    src/private/context.hpp
    src/private/first_order_types.hpp
    src/private/gen_alloca.hpp
    src/private/gen_array.hpp
    src/private/gen_attrs.hpp
    src/private/gen_body.hpp
    src/private/gen_builtin.hpp
    src/private/gen_func.hpp
    src/private/gen_loop.hpp
    src/private/gen_type.hpp
    src/private/gen_val.hpp
    src/private/llvm_func.hpp
    src/private/proto.hpp
    # src files
    src/context.cpp
    src/first_order_types.cpp
    src/gen.cpp
    src/gen_alloca.cpp
    src/gen_array.cpp
    src/gen_attrs.cpp
    src/gen_body.cpp
    src/gen_builtin.cpp
    src/gen_func.cpp
    src/gen_loop.cpp
    src/gen_type.cpp
    src/gen_val.cpp
    src/llvm_func.cpp
    src/proto.cpp
    )
add_library(DepC::Dep0::LLVMGen ALIAS dep0_llvmgen_lib)
target_compile_features(dep0_llvmgen_lib PUBLIC cxx_std_20)
target_include_directories(dep0_llvmgen_lib PUBLIC include PRIVATE src)
target_link_libraries(dep0_llvmgen_lib
  PUBLIC
    DepC::Dep0::TypeCheck
    ${LLVM_COMPONENTS})
