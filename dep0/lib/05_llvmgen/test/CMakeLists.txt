add_custom_target(dep0_llvmgen_tests)

add_library(dep0_llvmgen_tests_fixture
    llvmgen_tests_fixture.hpp
    llvmgen_tests_fixture.cpp
    llvm_predicates_v2/add.hpp
    llvm_predicates_v2/add.cpp
    llvm_predicates_v2/align.hpp
    llvm_predicates_v2/align.cpp
    llvm_predicates_v2/alloca.hpp
    llvm_predicates_v2/alloca.cpp
    llvm_predicates_v2/and.hpp
    llvm_predicates_v2/and.cpp
    llvm_predicates_v2/argument.hpp
    llvm_predicates_v2/argument.cpp
    llvm_predicates_v2/bitcast.hpp
    llvm_predicates_v2/bitcast.cpp
    llvm_predicates_v2/branch.hpp
    llvm_predicates_v2/branch.cpp
    llvm_predicates_v2/call.hpp
    llvm_predicates_v2/call.cpp
    llvm_predicates_v2/cmp.hpp
    llvm_predicates_v2/cmp.cpp
    llvm_predicates_v2/constants.hpp
    llvm_predicates_v2/constants.cpp
    llvm_predicates_v2/function.hpp
    llvm_predicates_v2/function.cpp
    llvm_predicates_v2/function_type.hpp
    llvm_predicates_v2/function_type.cpp
    llvm_predicates_v2/gep.hpp
    llvm_predicates_v2/gep.cpp
    llvm_predicates_v2/load.hpp
    llvm_predicates_v2/load.cpp
    llvm_predicates_v2/mul.hpp
    llvm_predicates_v2/mul.cpp
    llvm_predicates_v2/or.hpp
    llvm_predicates_v2/or.cpp
    llvm_predicates_v2/predicate.hpp
    llvm_predicates_v2/predicate.cpp
    llvm_predicates_v2/return.hpp
    llvm_predicates_v2/return.cpp
    llvm_predicates_v2/select.hpp
    llvm_predicates_v2/select.cpp
    llvm_predicates_v2/store.hpp
    llvm_predicates_v2/store.cpp
    llvm_predicates_v2/to_string.hpp
    llvm_predicates_v2/to_string.cpp
    llvm_predicates_v2/types.hpp
    llvm_predicates_v2/types.cpp
    llvm_predicates_v2/xor.hpp
    llvm_predicates_v2/xor.cpp
    llvm_predicates_v2.hpp
    llvm_predicates_v2.cpp
    )
target_include_directories(dep0_llvmgen_tests_fixture PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(dep0_llvmgen_tests_fixture
  PUBLIC
    DepC::Dep0::Parser
    DepC::Dep0::TypeCheck
    DepC::Dep0::LLVMGen
    DepC::Dep0::Transform
    DepC::Dep0::Testing
    Boost::Boost
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
