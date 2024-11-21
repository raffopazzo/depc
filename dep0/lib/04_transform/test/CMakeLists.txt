#
# Copyright Raffaele Rossi 2023 - 2024.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
#
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
