add_library(dep0_ast_tests_fixture
    ast_tests_fixture.hpp
    ast_tests_fixture.cpp)
target_link_libraries(dep0_ast_tests_fixture
  PUBLIC
    DepC::Dep0::Ast
    DepC::Dep0::Testing
    Boost::Boost
    )

add_executable(dep0_alpha_equivalence_tests dep0_alpha_equivalence_tests.cpp)
add_test(NAME dep0_alpha_equivalence_tests COMMAND dep0_alpha_equivalence_tests)
target_link_libraries(dep0_alpha_equivalence_tests PRIVATE dep0_ast_tests_fixture)

add_executable(dep0_beta_delta_reduction_tests dep0_beta_delta_reduction_tests.cpp)
add_test(NAME dep0_beta_delta_reduction_tests COMMAND dep0_beta_delta_reduction_tests)
target_link_libraries(dep0_beta_delta_reduction_tests PRIVATE dep0_ast_tests_fixture)

add_executable(dep0_delta_reduction_tests dep0_delta_reduction_tests.cpp)
add_test(NAME dep0_delta_reduction_tests COMMAND dep0_delta_reduction_tests)
target_link_libraries(dep0_delta_reduction_tests PRIVATE dep0_ast_tests_fixture)

add_executable(dep0_occurs_in_tests dep0_occurs_in_tests.cpp)
add_test(NAME dep0_occurs_in_tests COMMAND dep0_occurs_in_tests)
target_link_libraries(dep0_occurs_in_tests PRIVATE dep0_ast_tests_fixture)

add_executable(dep0_substitute_tests dep0_substitute_tests.cpp)
add_test(NAME dep0_substitute_tests COMMAND dep0_substitute_tests)
target_link_libraries(dep0_substitute_tests PRIVATE dep0_ast_tests_fixture)
