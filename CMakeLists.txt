cmake_minimum_required(VERSION 3.13.2)

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()
# To use TBB_ROOT in find_package
if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()
# option() is ignored if a normal variable with the same name is set
# used for setting option in gtest
if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()
# ExternalProject warning related to DOWNLOAD_EXTRACT_TIMESTAMP
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

project(SGEXT
  VERSION 0.9.17
  LANGUAGES CXX C)
message(STATUS "SGEXT version: ${SGEXT_VERSION}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(GNUInstallDirs) # Define CMAKE_INSTALL_xxx: LIBDIR, INCLUDEDIR
set(CMAKE_INSTALL_PYTHONLIBDIR sgext) # Install folder for python libs
# Copy python libs to a single location at build time (for ease of use)
set(CMAKE_BUILD_PYTHONLIBDIR ${PROJECT_BINARY_DIR}/sgext) # copy python libs at build time

include(CMakeDependentOption)
option(SG_BUILD_CLI "Build cpp-scripts with CLI executables." ON)
option(SG_BUILD_TESTING "Enable Tests" OFF)
option(SG_BUILD_TESTING_INTERACTIVE "If SG_BUILD_TESTING=ON, enables tests with interactive windows. Turn it OFF for CI." ON)
option(SG_BUILD_ENABLE_VALGRIND "Enable Valgrind as a memchecker for tests (require debug symbols)" OFF)
mark_as_advanced(SG_BUILD_ENABLE_VALGRIND)
option(SG_BUILD_ENABLE_CLANGTIDY "Enable clangtidy for tests. Populates CMAKE_CXX_CLANG_TIDY." OFF)
mark_as_advanced(SG_BUILD_ENABLE_CLANGTIDY)
option(SG_MODULE_SCRIPTS "High level functions (no CLI)." ON)
option(SG_MODULE_ANALYZE "Analyze the graph properties." ON)
option(SG_MODULE_COMPARE "Logic to compare and merge low and high information graphs." ON)
option(SG_MODULE_LOCATE "Use VTK KDtree for graph point location." ON)
option(SG_MODULE_VISUALIZE "Functions to visualize spatial graphs and histograms." ON)
option(SG_MODULE_GENERATE "Generate a graph with desired statistical parameters using simulated annealing." ON)
option(SG_MODULE_DYNAMICS "Perform coarse grained dynamics simulation in the network." ON)
option(SG_MODULE_TREE "Functions for tree-like graphs, for example study generations (bifurcations) from a root node." ON)
option(SG_MODULE_VISUALIZE_WITH_QT "Use QT for some visualizations. It will also enable SG_MODULE_VISUALIZE." OFF)
if(SG_MODULE_VISUALIZE_WITH_QT)
  set(SG_MODULE_VISUALIZE ON)
endif()
option(VISUALIZE "Enable visualization with VTK." OFF)
option(SG_ENABLE_ITK "Use ITK. Required for SCRIPTS, and optionally for VISUALIZE modules." OFF)
option(SG_WRAP_PYTHON "Use pybind11 to create bindings to python." OFF)

set(DEPENDENCIES_BUILD_DIR "" CACHE PATH "Base folder generated from building the sub-project ./dependencies. It contains boost-build, DGtal-build, etc.")
# The following script will set ITK_DIR, VTK_DIR DGtal_DIR, etc. Based on DEPENDENCIES_BUILD_DIR.
include(SGEXT_handle_dependencies)

if(VISUALIZE OR SG_MODULE_VISUALIZE OR SG_MODULE_LOCATE OR SG_MODULE_COMPARE)
  set(SG_REQUIRES_VTK TRUE)
endif()
if(SG_MODULE_SCRIPTS OR SG_MODULE_TREE OR SG_BUILD_CLI OR SG_ENABLE_ITK)
  set(SG_REQUIRES_ITK TRUE)
endif()

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 14)
    if(SG_MODULE_GENERATE OR SG_MODULE_DYNAMICS)
      set(CMAKE_CXX_STANDARD 17)
    endif()
endif()

find_package(Boost COMPONENTS
        program_options
        filesystem
        graph
        serialization
        REQUIRED)
message(STATUS "Boost_VERSION: ${Boost_VERSION}")
message(STATUS "Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")

find_package(DGtal 1.1 REQUIRED)
message(STATUS "DGtal version: ${DGtal_VERSION}")
message(STATUS "DGtal_DIR: ${DGtal_DIR}")

if(SG_REQUIRES_ITK)
  find_package(ITK CONFIG REQUIRED)
  include(${ITK_USE_FILE})

  set(ITKImageIOModule)
  if(${ITK_VERSION} VERSION_LESS "4.13.0")
    # Create list of avaialble image formats
    # From: https://discourse.itk.org/t/find-all-available-modules-from-groupio/351/5
    foreach( mod IN LISTS ITK_MODULES_ENABLED)
      if( ${mod} MATCHES "IO")
        list(APPEND ITKImageIOModule ${mod})
      endif()
    endforeach()
  else()
    set(ITKImageIOModule ITKImageIO)
  endif()

  set(ITKVtkGlueModule "")
  if(SG_REQUIRES_VTK)
    set(ITKVtkGlueModule ITKVtkGlue)
  endif()

  set(itk_components
    ITKCommon
    ITKIOImageBase
    ITKImageGrid
    ITKImageIntensity
    ITKImageStatistics
    # from segmentation_functions
    ITKDistanceMap
    ITKImageGradient
    ITKLevelSets
    ITKThresholding
    ITKRegionGrowing
    ${ITKVtkGlueModule}
    ${ITKImageIOModule}
    )
  find_package(ITK REQUIRED COMPONENTS
    ${itk_components}
    CONFIG
    )
  message(STATUS "ITK version: ${ITK_VERSION}")
  message(STATUS "ITK_DIR: ${ITK_DIR}")
  include(${ITK_USE_FILE})
endif() #ITK

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(SG_REQUIRES_VTK)
  set(_modern_VTK_available FALSE)
  set(_vtk_prefix "vtk")
  find_package(VTK REQUIRED)
  if(VTK_VERSION VERSION_GREATER_EQUAL 8.90.0)
    set(_modern_VTK_available TRUE)
    set(_vtk_prefix "")
  endif()
endif()

if(SG_MODULE_VISUALIZE_WITH_QT)
  if(UNIX)
    set(extra_qt_components X11Extras)
  endif()
  find_package(Qt5 REQUIRED
    Widgets
    Xml
    OpenGL
    ${extra_qt_components}
    )
  message(STATUS "Qt5 version: ${Qt5_VERSION}")

  set(vtk_qt_components
    ${_vtk_prefix}GUISupportQt # QVTKWidget.h
    )
endif()

if(SG_MODULE_VISUALIZE)
  set(vtk_components_visualize
    ${_vtk_prefix}InteractionStyle
    ${_vtk_prefix}RenderingContextOpenGL2 # avoid vtkContextDevice2D not found at run time
    ${_vtk_prefix}RenderingVolumeOpenGL2 # For volume rendering SmartVolumeMapper
    ${_vtk_prefix}ViewsContext2D
    ${_vtk_prefix}InteractionWidgets
    )
endif()

if(SG_REQUIRES_VTK)
  set(vtk_components
    ${_vtk_prefix}ChartsCore
    ${_vtk_prefix}CommonCore
    ${_vtk_prefix}CommonDataModel
    ${_vtk_prefix}ImagingStencil # For vtkPolyDataToImageStencil
    ${_vtk_prefix}ViewsInfovis
    ${_vtk_prefix}RenderingCore
    ${_vtk_prefix}RenderingOpenGL2 # vtkRenderer, vtkRenderWindow not found at run time
    ${_vtk_prefix}RenderingAnnotation
    ${_vtk_prefix}RenderingLabel
    ${_vtk_prefix}CommonColor
    ${_vtk_prefix}IOXML
    ${vtk_components_visualize}
    ${vtk_qt_components}
    )
  find_package(VTK REQUIRED COMPONENTS
    ${vtk_components}
    CONFIG
    )
  message(STATUS "VTK version: ${VTK_VERSION}")
  message(STATUS "VTK_DIR: ${VTK_DIR}")
  # USE_FILE is deprecated? Keep it. This is the cause of AUTOINIT warnings
  # https://gitlab.kitware.com/vtk/vtk/issues/15895
  if(NOT _modern_VTK_available)
    include(${VTK_USE_FILE})
  endif()
endif()

set(sgext_export_file "${PROJECT_BINARY_DIR}/SGEXTTargets.cmake")
# _targets_to_export holds all the targets that will exported to the build tree with export(TARGETS
set(_targets_to_export)

set(HISTO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/histo/include
        CACHE PATH "histo include directory")
set(HISTO_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/sgext)
install(DIRECTORY ${HISTO_INCLUDE_DIR}/ DESTINATION ${HISTO_INSTALL_INCLUDE_DIR})

# for convenience setup a target
add_library(histo INTERFACE)
add_library(SGEXT::histo ALIAS histo)
target_include_directories(histo INTERFACE
    $<BUILD_INTERFACE:${HISTO_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${HISTO_INSTALL_INCLUDE_DIR}>)

# need to export target as well
# this exports library: histo
install(TARGETS histo
  EXPORT SGEXTTargets
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Append to _targets_to_export for export to the build tree
set(_targets_to_export ${_targets_to_export} histo)

if(SG_BUILD_TESTING)
  if(SG_BUILD_ENABLE_VALGRIND)
    find_program(MEMORYCHECK_COMMAND valgrind)
    # set(MEMORYCHECK_SUPPRESSIONS_FILE "${CMAKE_CURRENT_BINARY_DIR}/valgrind_suppress.txt")
    set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
    include(Dart)
  endif()

  if(SG_BUILD_ENABLE_CLANGTIDY)
    find_program (CLANG_TIDY_EXE clang-tidy)
    set(CLANG_TIDY_CHECKS "\
    bugprone-*,-bugprone-exception-escape*,misc-*,\
    -misc-non-private-member-variables-in-classes*,\
    clang-analyzer-*,\
    modernize-*,-modernize-use-trailing-return-type*,-modernize-avoid-c-arrays*,\
    performance-*,portability-*,\
    readability-*,-readability-magic-numbers*,-readability-qualified-auto*,\
    -readability-isolate-declaration*,-readability-convert-member-functions-to-static*\
    ")
    set(_all_headers "modules/core/include/*" "modules/extract/include/*")
    if(SG_MODULE_VISUALIZE)
      list(APPEND _all_headers "modules/visualize/include/*")
    endif()
    if(SG_MODULE_ANALYZE)
      list(APPEND _all_headers "modules/analyze/include/*")
    endif()
    if(SG_MODULE_LOCATE)
      list(APPEND _all_headers "modules/locate/include/*")
    endif()
    if(SG_MODULE_COMPARE)
      list(APPEND _all_headers "modules/compare/include/*")
    endif()
    if(SG_MODULE_GENERATE)
      list(APPEND _all_headers "modules/generate/include/*")
    endif()
    if(SG_MODULE_DYNAMICS)
      list(APPEND _all_headers "modules/dynamics/include/*")
    endif()
    if(SG_REQUIRES_ITK)
      list(APPEND _all_headers "modules/itk_bridge/include/*")
    endif()
    if(SG_MODULE_TREE)
      list(APPEND _all_headers "modules/tree/include/*")
    endif()
    if(SG_MODULE_SCRIPTS)
      list(APPEND _all_headers "modules/scripts/include/*")
    endif()
    string(REPLACE ";" "," _all_headers_string "${_all_headers}")
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=${CLANG_TIDY_CHECKS};-header-filter=${_all_headers_string}")
    message(STATUS "Running clang-tidy with command: ${CMAKE_CXX_CLANG_TIDY}")
  endif()

  enable_testing()
  set(GTEST_LIBRARIES gtest gtest_main gmock)
  #############################################################################
  # Fetch GTest
  include(FetchContent)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
  )

  set(CMAKE_POLICY_DEFAULT_CMP0048 NEW) # google test raises warning about it
  set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # to enable override INSTALL_GTEST and gtest_force_shared_crt
  set(INSTALL_GTEST OFF)
  # Fix problems in windows with RuntimeLibrary
  # https://github.com/google/googletest/pull/1288/files
  # Needs cmake_policy CMP0077 NEW to have an effect.
  if (MSVC)
    set(gtest_force_shared_crt ON)
  endif()

  FetchContent_GetProperties(googletest)
  if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
  #############################################################################
  add_subdirectory(test/fixtures)
endif()

if(SG_WRAP_PYTHON)
  # Fetch pybind11
  include(FetchContent)
  FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11
    GIT_TAG v2.11.1
  )
  FetchContent_GetProperties(pybind11)
  if(NOT pybind11_POPULATED)
    FetchContent_Populate(pybind11)
    add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
  endif()
  # Export pybind to the build tree to facilitate external modules to reuse it
  # But not to the install tree.
  # External modules can link to SGEXT::pybind11 or SGEXT::module (only from the build tree)
  # And then:
  # include(${SGEXT_DIR}/_deps/pybind11-src/tools/pybind11NewTools.cmake)
  # to access the same pybind11_add_module CMake function.
  set(_targets_to_export ${_targets_to_export} pybind11::pybind11 pybind11::module pybind11_headers)
endif()

set(SG_LIBRARIES)
include(SGEXT_SGModuleMacros) # module macros
add_subdirectory(modules/core)
if(SG_MODULE_VISUALIZE)
  add_subdirectory(modules/visualize)
endif()
add_subdirectory(modules/extract) # it might use visualize
if(SG_MODULE_ANALYZE)
  add_subdirectory(modules/analyze)
endif()
if(SG_MODULE_LOCATE)
  add_subdirectory(modules/locate)
endif()
if(SG_MODULE_COMPARE)
  add_subdirectory(modules/compare)
endif()
if(SG_MODULE_GENERATE)
  add_subdirectory(modules/generate)
endif()
if(SG_MODULE_DYNAMICS)
  add_subdirectory(modules/dynamics)
endif()
if(SG_REQUIRES_ITK)
  add_subdirectory(modules/itk_bridge)
endif()
if(SG_MODULE_TREE)
  add_subdirectory(modules/tree)
endif()
if(SG_MODULE_SCRIPTS)
  add_subdirectory(modules/scripts)
endif()

if(SG_BUILD_CLI)
  add_subdirectory(cpp-scripts)
endif()

# WRAPPING
if(SG_WRAP_PYTHON)
  add_subdirectory(wrap)
endif()

# INSTALL
set(install_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/sgext")

install (EXPORT SGEXTTargets
  NAMESPACE SGEXT::
  DESTINATION ${install_cmake_dir} )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/SGEXTConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/SGEXTConfigVersion.cmake
              DESTINATION ${install_cmake_dir} )

# Export to build tree
list(REMOVE_DUPLICATES _targets_to_export)
export(TARGETS ${_targets_to_export}
       NAMESPACE SGEXT::
       FILE ${sgext_export_file})

include(CMakePackageConfigHelpers)

write_basic_package_version_file(SGEXTConfigVersion.cmake
  VERSION ${SGEXT_VERSION}
  COMPATIBILITY SameMajorVersion)

# Build tree
set(SGEXT_TARGETS_FILE ${sgext_export_file})
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SGEXTConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/SGEXTConfig.cmake
  INSTALL_DESTINATION ${install_cmake_dir}
  PATH_VARS SGEXT_TARGETS_FILE
  NO_CHECK_REQUIRED_COMPONENTS_MACRO # SGEXT does not provide components
  INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  )

# Install tree
set(SGEXT_TARGETS_FILE ${CMAKE_INSTALL_PREFIX}/${install_cmake_dir}/SGEXTTargets.cmake)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SGEXTConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/SGEXTConfig.cmake
  INSTALL_DESTINATION ${install_cmake_dir}
  PATH_VARS SGEXT_TARGETS_FILE
  NO_CHECK_REQUIRED_COMPONENTS_MACRO # SGEXT does not provide components
  )

# Add custom target to only install component: runtime (libraries)
add_custom_target(sgext-install-runtime
  ${CMAKE_COMMAND}
  -DCMAKE_INSTALL_COMPONENT=runtime
  -P "${PROJECT_BINARY_DIR}/cmake_install.cmake"
  DEPENDS ${SG_LIBRARIES}
  )
message(STATUS "SG_LIBRARIES: ${SG_LIBRARIES}")
add_dependencies(sgext-install-runtime ${SG_LIBRARIES})
if(SG_WRAP_PYTHON)
  add_dependencies(sgext-install-runtime _sgext)
endif()

# Copy to the build tree the cmake modules (to allow reuse from SGEXT_DIR after a find_package)
file(COPY
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SGEXT_SGModuleMacros.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SGEXT_handle_dependencies.cmake
 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/cmake)

install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SGEXT_SGModuleMacros.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SGEXT_handle_dependencies.cmake
  DESTINATION ${install_cmake_dir} )
