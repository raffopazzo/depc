add_executable(dep0_llvmgen_tests dep0_llvmgen_tests.cpp)
add_test(NAME dep0_llvmgen_tests COMMAND dep0_llvmgen_tests)
set_property(TEST dep0_llvmgen_tests PROPERTY ENVIRONMENT
    "DEP0_TESTFILES_DIR=${CMAKE_SOURCE_DIR}/testfiles"
    )
target_link_libraries(dep0_llvmgen_tests
  PRIVATE
    DepC::Dep0::Transform
    DepC::Dep0::LLVMGen
    Boost::Boost
    )
