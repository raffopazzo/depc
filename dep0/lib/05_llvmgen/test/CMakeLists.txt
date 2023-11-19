add_custom_target(dep0_llvmgen_tests)

add_library(dep0_llvmgen_tests_fixture
    llvmgen_tests_fixture.hpp
    llvmgen_tests_fixture.cpp)
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
