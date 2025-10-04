#
# Copyright Raffaele Rossi 2023 - 2024.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
#
find_package(Boost 1.85.0 REQUIRED)
find_package(llvm-core REQUIRED)
find_package(perfetto REQUIRED)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif()

add_subdirectory(app/dep0)

add_subdirectory(lib/00_antlr)
add_subdirectory(lib/00_core)
add_subdirectory(lib/01_ast)
add_subdirectory(lib/02_parser)
add_subdirectory(lib/03_typecheck)
add_subdirectory(lib/04_transform)
add_subdirectory(lib/05_llvmgen)
add_subdirectory(lib/06_compile)
add_subdirectory(lib/07_link)
add_subdirectory(lib/99_testing)
