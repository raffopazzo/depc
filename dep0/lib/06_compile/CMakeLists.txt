#
# Copyright Raffaele Rossi 2023 - 2024.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
#
add_library(dep0_compile_lib
    # public headers
    include/dep0/compile/compile.hpp
    # src files
    src/compile.cpp
    )
add_library(DepC::Dep0::Compile ALIAS dep0_compile_lib)
target_compile_features(dep0_compile_lib PUBLIC cxx_std_20)
target_include_directories(dep0_compile_lib PUBLIC include)
target_link_libraries(dep0_compile_lib
  PUBLIC
    DepC::Dep0::Core
    llvm-core::LLVMCore
  )
