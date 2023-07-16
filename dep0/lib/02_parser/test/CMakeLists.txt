add_executable(dep0_parser_tests dep0_parser_tests.cpp)
add_test(NAME dep0_parser_tests COMMAND dep0_parser_tests)
set_property(TEST dep0_parser_tests PROPERTY ENVIRONMENT
    "DEP0_TESTFILES_DIR=${CMAKE_SOURCE_DIR}/testfiles"
    )
target_link_libraries(dep0_parser_tests
  PRIVATE
    DepC::Dep0::Parser
    Boost::Boost
    )

add_executable(dep0_parser_properties_tests dep0_parser_properties_tests.cpp)
add_test(NAME dep0_parser_properties_tests COMMAND dep0_parser_properties_tests)
set_property(TEST dep0_parser_properties_tests PROPERTY ENVIRONMENT
    "DEP0_TESTFILES_DIR=${CMAKE_SOURCE_DIR}/testfiles"
    )
target_link_libraries(dep0_parser_properties_tests
  PRIVATE
    DepC::Dep0::Parser
    Boost::Boost
    )
