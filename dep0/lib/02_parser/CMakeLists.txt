#
# Copyright Raffaele Rossi 2023 - 2024.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
#
add_subdirectory(test)
add_library(dep0_parser_lib
  # public headers
  include/dep0/parser/ast.hpp
  include/dep0/parser/parse.hpp

  # private headers
  src/private/parse_cpp_int.hpp

  src/ast.cpp
  src/parse.cpp
  src/parse_cpp_int.cpp
  )
add_library(DepC::Dep0::Parser ALIAS dep0_parser_lib)
target_compile_features(dep0_parser_lib PUBLIC cxx_std_20)
target_include_directories(dep0_parser_lib PUBLIC include PRIVATE src)
target_link_libraries(dep0_parser_lib
  PUBLIC
    DepC::Dep0::Ast
  PRIVATE
    Boost::Boost
    DepC::Dep0::Antlr4
    )
