#
# Copyright Raffaele Rossi 2023 - 2024.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
#
add_subdirectory(test)
add_library(dep0_ast_lib
    include/dep0/ast/alpha_equivalence.hpp
    include/dep0/ast/alpha_equivalence_impl.hpp
    include/dep0/ast/ast.hpp
    include/dep0/ast/attribute.hpp
    include/dep0/ast/concepts.hpp
    include/dep0/ast/get_if_array.hpp
    include/dep0/ast/hash_code.hpp
    include/dep0/ast/max_index.hpp
    include/dep0/ast/max_index_impl.hpp
    include/dep0/ast/mutable.hpp
    include/dep0/ast/occurs_in.hpp
    include/dep0/ast/occurs_in_impl.hpp
    include/dep0/ast/pretty_print.hpp
    include/dep0/ast/pretty_print_impl.hpp
    include/dep0/ast/qty.hpp
    include/dep0/ast/rename.hpp
    include/dep0/ast/rename_impl.hpp
    include/dep0/ast/replace.hpp
    include/dep0/ast/replace_impl.hpp
    include/dep0/ast/sign.hpp
    include/dep0/ast/size.hpp
    include/dep0/ast/size_impl.hpp
    include/dep0/ast/width.hpp
    src/alpha_equivalence.cpp
    src/ast.cpp
    src/attribute.cpp
    src/concepts.cpp
    src/get_if_array.cpp
    src/hash_code.cpp
    src/max_index.cpp
    src/mutable.cpp
    src/occurs_in.cpp
    src/pretty_print.cpp
    src/qty.cpp
    src/rename.cpp
    src/replace.cpp
    src/sign.cpp
    src/size.cpp
    src/width.cpp
    )
add_library(DepC::Dep0::Ast ALIAS dep0_ast_lib)
target_compile_features(dep0_ast_lib PUBLIC cxx_std_20)
target_include_directories(dep0_ast_lib PUBLIC include)
target_link_libraries(dep0_ast_lib
  PUBLIC
    DepC::Dep0::Core
    Boost::Boost
    )
