add_executable(dep0_beta_delta_reduction_tests dep0_beta_delta_reduction_tests.cpp)
add_test(NAME dep0_beta_delta_reduction_tests COMMAND dep0_beta_delta_reduction_tests)
target_link_libraries(dep0_beta_delta_reduction_tests
  PRIVATE
    DepC::Dep0::Ast
    DepC::Dep0::Testing
    Boost::Boost
    )

add_executable(dep0_delta_reduction_tests dep0_delta_reduction_tests.cpp)
add_test(NAME dep0_delta_reduction_tests COMMAND dep0_delta_reduction_tests)
target_link_libraries(dep0_delta_reduction_tests
  PRIVATE
    DepC::Dep0::Ast
    DepC::Dep0::Testing
    Boost::Boost
    )
