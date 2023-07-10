find_package(LLVM REQUIRED)

add_executable(dep0 main.cpp)
target_compile_features(dep0 PRIVATE cxx_std_20)
target_link_libraries(dep0
  PRIVATE
    DepC::Dep0::Core
    DepC::Dep0::Parser
    DepC::Dep0::TypeCheck
    DepC::Dep0::Transform
    DepC::Dep0::LLVMGen
    ${LLVM_COMPONENTS}
    )
