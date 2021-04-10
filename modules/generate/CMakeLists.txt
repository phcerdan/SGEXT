if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(_compiler_is_msvc ON)
  set(_parallel_stl_extra_libraries)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(_compiler_is_gnu ON)
  set(OLD_CMAKE_FIND_PACKAGE_PREFER_CONFIG ${CMAKE_FIND_PACKAGE_PREFER_CONFIG})
  set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)
  set(TBB_FIND_RELEASE_ONLY ON)
  find_package(TBB QUIET ) # For c++17 std::execution::par_unseq in gcc
  if(TBB_FOUND)
    if(TARGET TBB::tbb)
      set(_parallel_stl_extra_libraries TBB::tbb)
    else()
      set(_parallel_stl_extra_libraries ${TBB_LIBRARIES})
    endif()
  endif()
  set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ${OLD_CMAKE_FIND_PACKAGE_PREFER_CONFIG})
  message(STATUS "TBB_FOUND: ${TBB_FOUND}. tbb_libraries: ${_parallel_stl_extra_libraries}")
endif()

try_compile(_has_parallel_stl
  ${CMAKE_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/cmake/try_compile_execution_header.cpp
  LINK_LIBRARIES  ${_parallel_stl_extra_libraries}
  )

if(_has_parallel_stl)
  message(STATUS "module generate -- stl header <execution> is available.")
  if(_compiler_is_gnu)
    message(STATUS "Using TBB from ${TBB_INCLUDE_DIR}")
  endif()
else()
  if(_compiler_is_gnu AND ${CMAKE_CXX_STANDARD} GREATER_EQUAL 17)
    message(STATUS "module generate -- can optionally use TBB to use parallel algorithms in GCC. Provide -DTBB_DIR to enable it.")
  endif()
endif()
# Fetch perm-montecarlo
include(FetchContent)
set(PERM_WRAP_PYTHON 0)
FetchContent_Declare(
  perm-montecarlo
  GIT_REPOSITORY https://github.com/phcerdan/perm-montecarlo
  # GIT_TAG 2cac20b700c6a5ff85b8af545a6fd82ef4e70b11
  GIT_TAG master
)
FetchContent_GetProperties(perm-montecarlo)
if(NOT perm-montecarlo_POPULATED)
  FetchContent_Populate(perm-montecarlo)
  add_subdirectory(${perm-montecarlo_SOURCE_DIR} ${perm-montecarlo_BINARY_DIR})
endif()

# need to export target as well
# this exports library: PERMCore
install(TARGETS PERMCore
  EXPORT SGEXTTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT development
  )

# export to the build tree
export(TARGETS PERMCore
  NAMESPACE SGEXT::
  APPEND FILE ${sgext_export_file})

# # Use FLANN for collision detector (kd-tree)
# option(FLANN_BUILD_C_BINDINGS "Build C bindings" OFF)
# option(FLANN_BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)
# option(FLANN_BUILD_MATLAB_BINDINGS "Build Matlab bindings" OFF)
# option(FLANN_BUILD_CUDA_LIB "Build CUDA library" OFF)
# option(FLANN_BUILD_EXAMPLES "Build examples" OFF)
# option(FLANN_BUILD_TESTS "Build tests" OFF)
# option(FLANN_BUILD_DOC "Build documentation" OFF)
# option(FLANN_USE_OPENMP "Use OpenMP multi-threading" OFF)
# option(FLANN_USE_MPI "Use MPI" OFF)
# mark_as_advanced(FLANN_BUILD_C_BINDINGS)
# mark_as_advanced(FLANN_BUILD_PYTHON_BINDINGS)
# mark_as_advanced(FLANN_BUILD_MATLAB_BINDINGS)
# mark_as_advanced(FLANN_BUILD_CUDA_LIB)
# mark_as_advanced(FLANN_BUILD_EXAMPLES)
# mark_as_advanced(FLANN_BUILD_TESTS)
# mark_as_advanced(FLANN_BUILD_DOC)
# mark_as_advanced(FLANN_USE_OPENMP)
# mark_as_advanced(FLANN_USE_MPI)
#
# # Make copies of variables that might be defined for this project
# set(BUILD_TESTS_THIS_PROJECT ${BUILD_TESTS})
# set(BUILD_DOC_THIS_PROJECT ${BUILD_DOC})
#
# set(BUILD_C_BINDINGS ${FLANN_BUILD_C_BINDINGS} CACHE INTERNAL "")
# set(BUILD_PYTHON_BINDINGS ${FLANN_BUILD_PYTHON_BINDINGS} CACHE INTERNAL "")
# set(BUILD_MATLAB_BINDINGS ${FLANN_BUILD_MATLAB_BINDINGS} CACHE INTERNAL "")
# set(BUILD_CUDA_LIB ${FLANN_BUILD_CUDA_LIB} CACHE INTERNAL "")
# set(BUILD_EXAMPLES ${FLANN_BUILD_EXAMPLES} CACHE INTERNAL "")
# set(BUILD_TESTS ${FLANN_BUILD_TESTS} CACHE INTERNAL "")
# set(BUILD_DOC ${FLANN_BUILD_DOC} CACHE INTERNAL "")
# set(USE_OPENMP ${FLANN_USE_OPENMP} CACHE INTERNAL "")
# set(USE_MPI ${FLANN_USE_MPI} CACHE INTERNAL "")
# FetchContent_Declare(
#   flann
#   GIT_REPOSITORY https://github.com/phcerdan/flann
#   GIT_TAG master
# )
# FetchContent_GetProperties(flann)
# if(NOT flann_POPULATED)
#   FetchContent_Populate(flann)
#   add_subdirectory(${flann_SOURCE_DIR} ${flann_BINARY_DIR})
# endif()
# # Restore variables that collided with flann
# set(BUILD_TESTS ${BUILD_TESTS_THIS_PROJECT})
# set(BUILD_DOC ${BUILD_DOC_THIS_PROJECT})
# unset(BUILD_TESTS_THIS_PROJECT)
# unset(BUILD_DOC_THIS_PROJECT)

set(SG_MODULE_NAME "Generate")
set(SG_MODULE_${SG_MODULE_NAME}_LIBRARY "SG${SG_MODULE_NAME}")
set(SG_LIBRARIES ${SG_LIBRARIES} ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PARENT_SCOPE)
set(SG_MODULE_INTERNAL_DEPENDS
  SGCore
  PERMCore # From perm-montecarlo
  )
set(_optional_depends "")
if(_has_parallel_stl AND _compiler_is_gnu)
  set(SG_REQUIRES_TBB TRUE PARENT_SCOPE)
  list(APPEND _optional_depends ${_parallel_stl_extra_libraries})
endif()
set(SG_MODULE_${SG_MODULE_NAME}_DEPENDS
  ${SG_MODULE_INTERNAL_DEPENDS}
  ${_optional_depends}
  histo)
set(SG_MODULE_${SG_MODULE_NAME}_SOURCES
    generate_common.cpp
    simulated_annealing_generator.cpp
    simulated_annealing_generator_config_tree.cpp
    update_step.cpp
    update_step_move_node.cpp
    update_step_swap_edges.cpp
    update_step_generate_contour_length.cpp
    contour_length_generator.cpp
    contour_length_generator_functions.cpp
    degree_viger_generator.cpp
    degree_sequences.cpp
    )
list(TRANSFORM SG_MODULE_${SG_MODULE_NAME}_SOURCES PREPEND "src/")
add_library(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} ${SG_MODULE_${SG_MODULE_NAME}_SOURCES})
add_library(SGEXT::${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} ALIAS ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY})
target_include_directories(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

if(_has_parallel_stl)
  target_compile_definitions(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PUBLIC -DWITH_PARALLEL_STL)
  if(_compiler_is_gnu)
    target_include_directories(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PUBLIC ${TBB_INCLUDE_DIR})
  endif()
endif()

target_link_libraries(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  )

if(SG_BUILD_TESTING)
  add_subdirectory(test)
endif()

install(TARGETS ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
        EXPORT SGEXTTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT development
        )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sgext
  PATTERN "*.txt" EXCLUDE)

# export to the build tree
export( TARGETS ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  NAMESPACE SGEXT::
  APPEND FILE ${sgext_export_file})

if(SG_BUILD_CLI)
  add_subdirectory(bin)
endif()
