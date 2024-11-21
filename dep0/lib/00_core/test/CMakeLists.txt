#
# Copyright Raffaele Rossi 2023 - 2024.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
#
add_custom_target(dep0_core_tests)

macro(add_dep0_core_test name)
  add_executable(dep0_core_${name}_tests dep0_core_${name}_tests.cpp)
  add_test(NAME dep0_core_${name}_tests COMMAND dep0_core_${name}_tests)
  add_dependencies(dep0_core_tests dep0_core_${name}_tests)
  target_link_libraries(dep0_core_${name}_tests PUBLIC DepC::Dep0::Core Boost::Boost)
endmacro()

add_dep0_core_test(error)
add_dep0_core_test(match)
add_dep0_core_test(scope_map)
add_dep0_core_test(vector_splice)
