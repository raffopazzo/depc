add_executable(dep0
  main.cpp
  # hdr
  failure.hpp
  job.hpp
  pipeline.hpp
  # src
  failure.cpp
  job.cpp
  pipeline.cpp
  )
target_compile_features(dep0 PRIVATE cxx_std_20)
target_link_libraries(dep0
  PRIVATE
    DepC::Dep0::Core
    DepC::Dep0::Parser
    DepC::Dep0::TypeCheck
    DepC::Dep0::Transform
    DepC::Dep0::LLVMGen
    DepC::Dep0::Compile
    DepC::Dep0::Link
    ${LLVM_COMPONENTS}
    )
