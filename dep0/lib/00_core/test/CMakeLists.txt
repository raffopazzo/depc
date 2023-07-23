add_executable(dep0_core_tests dep0_core_tests.cpp)
add_test(NAME dep0_core_tests COMMAND dep0_core_tests)
target_link_libraries(dep0_core_tests
  PRIVATE
    DepC::Dep0::Core
    Boost::Boost
    )

add_executable(dep0_core_match_tests dep0_core_match_tests.cpp)
add_test(NAME dep0_core_match_tests COMMAND dep0_core_match_tests)
target_link_libraries(dep0_core_match_tests
  PRIVATE
    DepC::Dep0::Core
    Boost::Boost
    )
