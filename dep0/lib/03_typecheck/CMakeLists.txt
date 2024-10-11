add_subdirectory(test)

add_library(dep0_typecheck_lib
  # public headers
  include/dep0/typecheck/ast.hpp
  include/dep0/typecheck/beta_delta_reduction.hpp
  include/dep0/typecheck/check.hpp
  include/dep0/typecheck/context.hpp
  include/dep0/typecheck/derivation.hpp
  include/dep0/typecheck/environment.hpp
  include/dep0/typecheck/is_impossible.hpp
  include/dep0/typecheck/is_mutable.hpp
  include/dep0/typecheck/list_initialization.hpp
  # private headers
  src/private/beta_delta_equivalence.hpp
  src/private/beta_reduction.hpp
  src/private/check.hpp
  src/private/cpp_int_add.hpp
  src/private/cpp_int_limits.hpp
  src/private/cpp_int_sub.hpp
  src/private/c_types.hpp
  src/private/delta_unfold.hpp
  src/private/derivation_rules.hpp
  src/private/drop_unreachable_stmts.hpp
  src/private/is_terminator.hpp
  src/private/prelude.hpp
  src/private/proof_search.hpp
  src/private/proof_state.hpp
  src/private/returns_from_all_branches.hpp
  src/private/rewrite.hpp
  src/private/substitute.hpp
  src/private/unification.hpp
  src/private/usage.hpp
  src/private/type_assign.hpp
  # source files
  src/ast.cpp
  src/beta_delta_equivalence.cpp
  src/beta_delta_reduction.cpp
  src/beta_reduction.cpp
  src/check.cpp
  src/context.cpp
  src/cpp_int_add.cpp
  src/cpp_int_limits.cpp
  src/cpp_int_sub.cpp
  src/c_types.cpp
  src/delta_unfold.cpp
  src/derivation.cpp
  src/derivation_rules.cpp
  src/drop_unreachable_stmts.cpp
  src/environment.cpp
  src/is_impossible.cpp
  src/is_mutable.cpp
  src/is_terminator.cpp
  src/list_initialization.cpp
  src/prelude.cpp
  src/proof_search.cpp
  src/proof_state.cpp
  src/returns_from_all_branches.cpp
  src/rewrite.cpp
  src/substitute.cpp
  src/unification.cpp
  src/usage.cpp
  src/type_assign.cpp
  # tactics headers
  src/private/tactics/search_app.hpp
  src/private/tactics/search_trivial_value.hpp
  src/private/tactics/search_true_t.hpp
  src/private/tactics/search_var.hpp
  # tactics sources
  src/tactics/search_app.cpp
  src/tactics/search_trivial_value.cpp
  src/tactics/search_true_t.cpp
  src/tactics/search_var.cpp
  )
add_library(DepC::Dep0::TypeCheck ALIAS dep0_typecheck_lib)
target_compile_features(dep0_typecheck_lib PUBLIC cxx_std_20)
target_include_directories(dep0_typecheck_lib PUBLIC include PRIVATE src)
target_link_libraries(dep0_typecheck_lib
  PUBLIC
    DepC::Dep0::Ast
    DepC::Dep0::Parser
  )
