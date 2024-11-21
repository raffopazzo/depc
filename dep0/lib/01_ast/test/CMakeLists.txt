#
# Copyright Raffaele Rossi 2023 - 2024.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
#
add_custom_target(dep0_ast_tests)

add_library(dep0_ast_tests_fixture
    ast_tests_fixture.hpp
    ast_tests_fixture.cpp)
target_link_libraries(dep0_ast_tests_fixture
  PUBLIC
    DepC::Dep0::Ast
    DepC::Dep0::Testing
    Boost::Boost
    )

macro(add_dep0_ast_test name)
  add_executable(dep0_${name}_tests dep0_${name}_tests.cpp)
  add_test(NAME dep0_${name}_tests COMMAND dep0_${name}_tests)
  target_link_libraries(dep0_${name}_tests PRIVATE dep0_ast_tests_fixture)
  add_dependencies(dep0_ast_tests dep0_${name}_tests)
endmacro()
add_dep0_ast_test(alpha_equivalence)
add_dep0_ast_test(hash_code)
add_dep0_ast_test(occurs_in)
