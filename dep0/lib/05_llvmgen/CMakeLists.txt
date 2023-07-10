add_subdirectory(test)

find_package(LLVM REQUIRED)
add_library(dep0_llvmgen_lib
    include/dep0/llvmgen/gen.hpp
    src/gen.cpp
    )
add_library(DepC::Dep0::LLVMGen ALIAS dep0_llvmgen_lib)
target_compile_features(dep0_llvmgen_lib PUBLIC cxx_std_20)
target_include_directories(dep0_llvmgen_lib PUBLIC include)
target_link_libraries(dep0_llvmgen_lib
  PUBLIC
    DepC::Dep0::Parser
    DepC::Dep0::TypeCheck
    LLVM::LLVMCore)
