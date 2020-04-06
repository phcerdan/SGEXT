# Dependencies
find_package(TBB REQUIRED) # For c++17 std::execution::par_unseq
# Fetch perm-montecarlo
include(FetchContent)
set(PERM_WRAP_PYTHON ${SG_WRAP_PYTHON})
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
list(APPEND SG_LIBRARIES ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY})
set(SG_MODULE_INTERNAL_DEPENDS
  SGCore
  PERMCore # From perm-montecarlo
  )
set(SG_MODULE_${SG_MODULE_NAME}_DEPENDS
  ${SG_MODULE_INTERNAL_DEPENDS}
  ${TBB_LIBRARIES}
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
target_include_directories(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_include_directories(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PUBLIC ${TBB_INCLUDE_DIR})

target_link_libraries(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  )

if(SG_BUILD_TESTING)
  add_subdirectory(test)
endif()

install(TARGETS ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
        EXPORT SGEXTTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sgext
  PATTERN "*.txt" EXCLUDE)

# export to the build tree
export( TARGETS ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  NAMESPACE SGEXT::
  APPEND FILE ${sgext_export_file})

if(SG_BUILD_SCRIPTS)
  add_subdirectory(bin)
endif()
