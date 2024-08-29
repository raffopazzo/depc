# add_subdirectory(test)

find_package(LLVM REQUIRED)
add_library(dep0_linker_lib
    # public headers
    include/dep0/linker/link.hpp
    # private headers
    src/private/x86_64_linux.hpp
    # src files
    src/link.cpp
    src/x86_64_linux.cpp
    )
add_library(DepC::Dep0::Linker ALIAS dep0_linker_lib)
target_compile_features(dep0_linker_lib PUBLIC cxx_std_20)
target_include_directories(dep0_linker_lib PUBLIC include PRIVATE src)
target_link_libraries(dep0_linker_lib
  PUBLIC
    DepC::Dep0::Parser
    DepC::Dep0::TypeCheck
    LLVM::LLVMCore
    LLVM::LLVMTransformUtils
  PRIVATE
    Boost::Boost
  )
