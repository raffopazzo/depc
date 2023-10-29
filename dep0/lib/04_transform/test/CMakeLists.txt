add_executable(dep0_transform_tests dep0_transform_tests.cpp)
add_test(NAME dep0_transform_tests COMMAND dep0_transform_tests)
set_property(TEST dep0_transform_tests PROPERTY ENVIRONMENT
    "DEP0_TESTFILES_DIR=${CMAKE_SOURCE_DIR}/testfiles"
    )
target_link_libraries(dep0_transform_tests
  PRIVATE
    DepC::Dep0::Transform
    DepC::Dep0::Testing
    Boost::Boost
    )
