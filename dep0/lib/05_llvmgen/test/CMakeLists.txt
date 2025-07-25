#
# Copyright Raffaele Rossi 2023 - 2025.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
#
add_custom_target(dep0_llvmgen_tests)

add_library(dep0_llvmgen_tests_fixture
    llvmgen_tests_fixture.hpp
    llvmgen_tests_fixture.cpp
    llvm_helpers.hpp
    llvm_helpers.cpp
    llvm_predicates/add.hpp
    llvm_predicates/add.cpp
    llvm_predicates/align.hpp
    llvm_predicates/align.cpp
    llvm_predicates/alloca.hpp
    llvm_predicates/alloca.cpp
    llvm_predicates/and.hpp
    llvm_predicates/and.cpp
    llvm_predicates/argument.hpp
    llvm_predicates/argument.cpp
    llvm_predicates/bitcast.hpp
    llvm_predicates/bitcast.cpp
    llvm_predicates/branch.hpp
    llvm_predicates/branch.cpp
    llvm_predicates/call.hpp
    llvm_predicates/call.cpp
    llvm_predicates/cmp.hpp
    llvm_predicates/cmp.cpp
    llvm_predicates/constants.hpp
    llvm_predicates/constants.cpp
    llvm_predicates/cstrings.hpp
    llvm_predicates/cstrings.cpp
    llvm_predicates/function.hpp
    llvm_predicates/function.cpp
    llvm_predicates/function_type.hpp
    llvm_predicates/function_type.cpp
    llvm_predicates/gep.hpp
    llvm_predicates/gep.cpp
    llvm_predicates/global.hpp
    llvm_predicates/global.cpp
    llvm_predicates/intrinsics.hpp
    llvm_predicates/intrinsics.cpp
    llvm_predicates/load.hpp
    llvm_predicates/load.cpp
    llvm_predicates/mul.hpp
    llvm_predicates/mul.cpp
    llvm_predicates/or.hpp
    llvm_predicates/or.cpp
    llvm_predicates/phi.hpp
    llvm_predicates/phi.cpp
    llvm_predicates/predicate.hpp
    llvm_predicates/predicate.cpp
    llvm_predicates/return.hpp
    llvm_predicates/return.cpp
    llvm_predicates/select.hpp
    llvm_predicates/select.cpp
    llvm_predicates/store.hpp
    llvm_predicates/store.cpp
    llvm_predicates/sub.hpp
    llvm_predicates/sub.cpp
    llvm_predicates/to_string.hpp
    llvm_predicates/to_string.cpp
    llvm_predicates/types.hpp
    llvm_predicates/types.cpp
    llvm_predicates/unreachable.hpp
    llvm_predicates/unreachable.cpp
    llvm_predicates/xor.hpp
    llvm_predicates/xor.cpp
    llvm_predicates/zeroinitializer.hpp
    llvm_predicates/zeroinitializer.cpp
    llvm_predicates.hpp
    llvm_predicates.cpp
    )
target_include_directories(dep0_llvmgen_tests_fixture PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(dep0_llvmgen_tests_fixture
  PUBLIC
    DepC::Dep0::Ast
    DepC::Dep0::Parser
    DepC::Dep0::TypeCheck
    DepC::Dep0::LLVMGen
    DepC::Dep0::Transform
    DepC::Dep0::Testing
    Boost::Boost
    ${LLVM_COMPONENTS}
    )

macro(add_dep0_llvmgen_test name)
  add_executable(dep0_llvmgen_tests_${name} dep0_llvmgen_tests_${name}.cpp)
  add_test(NAME dep0_llvmgen_tests_${name} COMMAND dep0_llvmgen_tests_${name})
  set_property(TEST dep0_llvmgen_tests_${name} PROPERTY ENVIRONMENT "DEP0_TESTFILES_DIR=${CMAKE_SOURCE_DIR}/testfiles")
  target_link_libraries(dep0_llvmgen_tests_${name} PRIVATE dep0_llvmgen_tests_fixture)
  add_dependencies(dep0_llvmgen_tests dep0_llvmgen_tests_${name})
endmacro()
add_dep0_llvmgen_test(0000_basics)
add_dep0_llvmgen_test(0001_integral_primitives)
add_dep0_llvmgen_test(0002_user_defined_integrals)
add_dep0_llvmgen_test(0003_function_arguments)
add_dep0_llvmgen_test(0004_plus_expr)
add_dep0_llvmgen_test(0005_pi_types)
add_dep0_llvmgen_test(0006_kinds)
add_dep0_llvmgen_test(0007_arrays)
add_dep0_llvmgen_test(0008_boolean_expr)
add_dep0_llvmgen_test(0009_func_decl)
add_dep0_llvmgen_test(0010_axioms)
add_dep0_llvmgen_test(0011_quantities)
add_dep0_llvmgen_test(0012_auto_expr)
add_dep0_llvmgen_test(0013_cstr)
add_dep0_llvmgen_test(0014_extern_decl)
add_dep0_llvmgen_test(0015_because)
add_dep0_llvmgen_test(0016_impossible)
add_dep0_llvmgen_test(0016_prelude)
add_dep0_llvmgen_test(0017_minus_expr)
add_dep0_llvmgen_test(0018_mult_expr)
add_dep0_llvmgen_test(0019_div_expr)
add_dep0_llvmgen_test(0020_builtins)
add_dep0_llvmgen_test(0021_tuples)
add_dep0_llvmgen_test(0022_structs)
add_dep0_llvmgen_test(0023_references)
