cmake_minimum_required(VERSION 3.8.1)

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

project(SGEXT
  VERSION 0.9.0
  LANGUAGES CXX)
message(STATUS "SGEXT version: ${SGEXT_VERSION}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(GNUInstallDirs) # Define CMAKE_INSTALL_xxx: LIBDIR, INCLUDEDIR
set(CMAKE_INSTALL_PYTHONLIBDIR sgext) # Install folder for python libs
# Copy python libs to a single location at build time (for ease of use)
set(CMAKE_BUILD_PYTHONLIBDIR ${PROJECT_BINARY_DIR}/sgext) # copy python libs at build time

include(CMakeDependentOption)
option(SG_BUILD_SCRIPTS "Build cpp-scripts." ON)
option(SG_BUILD_TESTING "Enable Tests" ON)
option(SG_BUILD_ENABLE_VALGRIND "Enable Valgrind as a memchecker for tests (require debug symbols)" OFF)
mark_as_advanced(SG_BUILD_ENABLE_VALGRIND)
option(SG_MODULE_ANALYZE "Analyze the graph properties." ON)
option(SG_MODULE_COMPARE "Logic to compare and merge low and high information graphs." ON)
option(SG_MODULE_LOCATE "Use VTK KDtree for graph point location." ON)
option(SG_MODULE_VISUALIZE "Functions to visualize spatial graphs and histograms." ON)
option(SG_MODULE_GENERATE "Generate a graph with desired statistical parameters using simulated annealing." ON)
option(SG_MODULE_DYNAMICS "Perform coarse grained dynamics simulation in the network." ON)
option(SG_MODULE_DYNAMICS_USING_VTK "Force enabling VTK in the dynamics module." OFF)
mark_as_advanced(SG_MODULE_DYNAMICS_USING_VTK)
option(SG_MODULE_VISUALIZE_WITH_QT "Use QT for some visualizations. It will also enable SG_MODULE_VISUALIZE." OFF)
if(SG_MODULE_VISUALIZE_WITH_QT)
  set(SG_MODULE_VISUALIZE ON)
endif()
option(VISUALIZE "Enable visualization with VTK." OFF)
option(SG_ENABLE_ITK "Use ITK. Required for SCRIPTS, and optionally for VISUALIZE modules." ON)
option(SG_WRAP_PYTHON "Use pybind11 to create bindings to python." OFF)


if(VISUALIZE OR SG_MODULE_VISUALIZE OR SG_MODULE_LOCATE OR SG_MODULE_COMPARE OR SG_MODULE_DYNAMICS_USING_VTK)
  set(SG_REQUIRES_VTK TRUE)
endif()
if(SG_BUILD_SCRIPTS OR SG_ENABLE_ITK)
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

find_package(DGtal REQUIRED 1.0)
message(STATUS "DGtal version: ${DGtal_VERSION}")

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
    ${ITKVtkGlueModule}
    ${ITKImageIOModule}
    )
  find_package(ITK REQUIRED COMPONENTS
    ${itk_components}
    CONFIG
    )
  message(STATUS "ITK version: ${ITK_VERSION}")
  include(${ITK_USE_FILE})
endif() #ITK

set(vtk_qt_components)
if(SG_MODULE_VISUALIZE_WITH_QT)
  find_package(Qt5 REQUIRED
    Widgets
    Xml
    OpenGL)
  message(STATUS "Qt5 version: ${Qt5_VERSION}")

  if(Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()
  set(vtk_qt_components
    vtkGUISupportQt # QVTKWidget.h
    )
endif()

set(_modern_VTK_available 0)
if(SG_REQUIRES_VTK)
  set(vtk_components
    vtkChartsCore
    vtkCommonCore
    vtkCommonDataModel
    vtkInteractionStyle
    vtkRenderingCore
    vtkRenderingOpenGL2 # avoid vtkRenderer, vtkRenderWindow not found at run time
    vtkRenderingContextOpenGL2 # avoid vtkContextDevice2D not found at run time
    vtkRenderingVolumeOpenGL2 # For volume rendering SmartVolumeMapper
    vtkViewsContext2D
    vtkInteractionWidgets
    vtkViewsInfovis
    ${vtk_qt_components}
    )
  # VTK components to handle external dependency histo-header
  find_package(VTK REQUIRED COMPONENTS
    ${vtk_components}
    CONFIG
    )
  message(STATUS "VTK version: ${VTK_VERSION}")
  message(STATUS "VTK_DIR: ${VTK_DIR}")
  # USE_FILE is deprecated? Keep it. This is the cause of AUTOINIT warnings
  # https://gitlab.kitware.com/vtk/vtk/issues/15895
  include(${VTK_USE_FILE})
  if("${VTK_VERSION}" VERSION_GREATER "8.1") # Requires OpenGL2
    set(_modern_VTK_available 1)
  endif()
endif()

set(sgext_export_file "${PROJECT_BINARY_DIR}/SGEXTTargets.cmake")

set(HISTO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/histo/include
        CACHE PATH "histo include directory")
set(HISTO_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/sgext)
install(DIRECTORY ${HISTO_INCLUDE_DIR}/ DESTINATION ${HISTO_INSTALL_INCLUDE_DIR})

# for convenience setup a target
add_library(histo INTERFACE)
target_include_directories(histo INTERFACE
    $<BUILD_INTERFACE:${HISTO_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${HISTO_INSTALL_INCLUDE_DIR}>)

# need to export target as well
# this exports library: histo
install(TARGETS histo
  EXPORT SGEXTTargets
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# export to the build tree
export(TARGETS histo
  NAMESPACE SGEXT::
  APPEND FILE ${sgext_export_file})

if(SG_BUILD_TESTING)
  if(SG_BUILD_ENABLE_VALGRIND)
    find_program(MEMORYCHECK_COMMAND valgrind)
    set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
    set(MEMORYCHECK_SUPPRESSIONS_FILE "${CMAKE_CURRENT_BINARY_DIR}/valgrind_suppress.txt")
    include(Dart)
  endif()
  enable_testing()
  set(GTEST_LIBRARIES gtest gtest_main gmock)
  include(GoogleTest)
  #############################################################################
  # Fetch GTest
  include(FetchContent)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.8.x
  )

  set(CMAKE_POLICY_DEFAULT_CMP0048 NEW) # google test raises warning about it
  FetchContent_GetProperties(googletest)
  if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
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
    GIT_TAG v2.4.3
  )
  FetchContent_GetProperties(pybind11)
  if(NOT pybind11_POPULATED)
    FetchContent_Populate(pybind11)
    add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
  endif()
endif()

set(SG_LIBRARIES "")
include(SGModuleMacros) # module macros
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

if(SG_BUILD_SCRIPTS)
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

