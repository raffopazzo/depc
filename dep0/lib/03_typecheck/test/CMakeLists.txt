add_custom_target(dep0_typecheck_tests)

add_library(dep0_typecheck_tests_fixture
    typecheck_tests_fixture.hpp
    typecheck_tests_fixture.cpp)
target_link_libraries(dep0_typecheck_tests_fixture
  PUBLIC
    DepC::Dep0::Parser
    DepC::Dep0::TypeCheck
    DepC::Dep0::Testing
    Boost::Boost
    )

macro(add_dep0_typecheck_test name)
  add_executable(dep0_typecheck_tests_${name} dep0_typecheck_tests_${name}.cpp)
  add_test(NAME dep0_typecheck_tests_${name} COMMAND dep0_typecheck_tests_${name})
  set_property(TEST dep0_typecheck_tests_${name} PROPERTY ENVIRONMENT "DEP0_TESTFILES_DIR=${CMAKE_SOURCE_DIR}/testfiles")
  target_link_libraries(dep0_typecheck_tests_${name} PRIVATE dep0_typecheck_tests_fixture)
  add_dependencies(dep0_typecheck_tests dep0_typecheck_tests_${name})
endmacro()
add_dep0_typecheck_test(0000_basics)
add_dep0_typecheck_test(0001_integral_primitives)
add_dep0_typecheck_test(0002_user_defined_integrals)
add_dep0_typecheck_test(0003_function_arguments)
add_dep0_typecheck_test(0004_plus_expr)
add_dep0_typecheck_test(0005_pi_types)
add_dep0_typecheck_test(0006_kinds)
add_dep0_typecheck_test(0007_arrays)
add_dep0_typecheck_test(0008_boolean_expr)
add_dep0_typecheck_test(0009_func_decl)
add_dep0_typecheck_test(0010_axioms)
add_dep0_typecheck_test(0011_quantities)
add_dep0_typecheck_test(0012_auto_expr)
add_dep0_typecheck_test(0013_cstr)
add_dep0_typecheck_test(0014_extern_decl)
add_dep0_typecheck_test(0015_because)
add_dep0_typecheck_test(0016_impossible)
