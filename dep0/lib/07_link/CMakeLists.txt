add_library(dep0_link_lib
    # public headers
    include/dep0/link/link.hpp
    # private headers
    src/private/x86_64_linux.hpp
    # src files
    src/link.cpp
    src/x86_64_linux.cpp
    )
add_library(DepC::Dep0::Link ALIAS dep0_link_lib)
target_compile_features(dep0_link_lib PUBLIC cxx_std_20)
target_include_directories(dep0_link_lib PUBLIC include PRIVATE src)
target_link_libraries(dep0_link_lib
  PUBLIC
    DepC::Dep0::TypeCheck
    LLVM::LLVMCore
  PRIVATE
    Boost::Boost
  )
