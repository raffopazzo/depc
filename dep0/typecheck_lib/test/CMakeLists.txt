add_executable(dep0_typecheck_tests dep0_typecheck_tests.cpp)
add_test(NAME dep0_typecheck_tests COMMAND dep0_typecheck_tests)
set_property(TEST dep0_typecheck_tests PROPERTY ENVIRONMENT
    "DEP0_TESTFILES_DIR=${CMAKE_SOURCE_DIR}/testfiles"
    )
target_link_libraries(dep0_typecheck_tests
  PRIVATE
    DepC::Dep0::TypeCheck
    DepC::Dep0::Parser
    Boost::Boost
    )
