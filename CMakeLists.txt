cmake_minimum_required(VERSION 3.8.1)

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

project(SGEXT
  VERSION 0.8.0
  LANGUAGES CXX)
message(STATUS "SGEXT version: ${SGEXT_VERSION}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(CMakeDependentOption)
option(SG_BUILD_SCRIPTS "Build cpp-scripts." ON)
option(SG_BUILD_TESTING "Enable Tests" ON)
option(SG_MODULE_ANALYZE "Analyze the graph properties." ON)
option(SG_MODULE_COMPARE "Logic to compare and merge low and high information graphs." ON)
option(SG_MODULE_LOCATE "Use VTK KDtree for graph point location." ON)
option(SG_MODULE_VISUALIZE "Functions to visualize spatial graphs and histograms." ON)
CMAKE_DEPENDENT_OPTION(SG_MODULE_VISUALIZE_ENABLE_QT "Use QT for some visualizations" ON
  "SG_MODULE_VISUALIZE" ON)
option(VISUALIZE "Enable visualization with VTK." OFF)

if(VISUALIZE OR SG_MODULE_VISUALIZE OR SG_MODULE_LOCATE OR SG_MODULE_COMPARE)
  set(SG_REQUIRES_VTK TRUE)
endif()

find_package(Boost COMPONENTS
        program_options
        filesystem
        graph
        serialization
        REQUIRED)
# Requires version with this PR in: https://github.com/DGtal-team/DGtal/pull/1369
# Merged in DGtal 1.0.beta
find_package(DGtal REQUIRED 1.0)
message(STATUS "DGtal version: ${DGtal_VERSION}")

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

find_package(ITK REQUIRED COMPONENTS
  ITKCommon
  ITKIOImageBase
  ITKImageGrid
  ITKImageIntensity
  ITKImageStatistics
  ${ITKVtkGlueModule}
  ${ITKImageIOModule}
  CONFIG
)
message(STATUS "ITK version: ${ITK_VERSION}")
include(${ITK_USE_FILE})
set(vtk_qt_components)
if(SG_MODULE_VISUALIZE_ENABLE_QT)
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
  # VTK components to handle external dependency histo-header
  find_package(VTK REQUIRED COMPONENTS
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

set(HISTO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/histo/include
        CACHE PATH "histo include directory")
install(DIRECTORY ${HISTO_INCLUDE_DIR}/ DESTINATION include/sgext)

# for convenience setup a target
add_library(histo INTERFACE)
target_include_directories(histo INTERFACE
    $<BUILD_INTERFACE:${HISTO_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include/sgext>)

# need to export target as well
# this exports library: histo
install(TARGETS histo
  EXPORT SGEXTTargets
  DESTINATION include)

if(SG_BUILD_TESTING)
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

set(SG_LIBRARIES "")
set(sgext_export_file "${PROJECT_BINARY_DIR}/SGEXTTargets.cmake")
include(SGModuleMacros) # module macros
add_subdirectory(modules/core)
add_subdirectory(modules/extract)
if(SG_MODULE_ANALYZE)
  add_subdirectory(modules/analyze)
endif()
if(SG_MODULE_LOCATE)
  add_subdirectory(modules/locate)
endif()
if(SG_MODULE_COMPARE)
  add_subdirectory(modules/compare)
endif()
if(SG_MODULE_VISUALIZE)
  add_subdirectory(modules/visualize)
endif()

if(SG_BUILD_SCRIPTS)
  add_subdirectory(cpp-scripts)
endif()

install (EXPORT SGEXTTargets
  NAMESPACE SGEXT::
  DESTINATION lib/sgext/cmake )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/SGEXTConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/SGEXTConfigVersion.cmake
              DESTINATION lib/sgext/cmake )

include(CMakePackageConfigHelpers)

write_basic_package_version_file(SGEXTConfigVersion.cmake
  VERSION ${SGEXT_VERSION}
  COMPATIBILITY SameMajorVersion)

# Build tree
set(SGEXT_TARGETS_FILE ${sgext_export_file})
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SGEXTConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/SGEXTConfig.cmake
  INSTALL_DESTINATION lib/sgext/cmake
  PATH_VARS SGEXT_TARGETS_FILE
  NO_CHECK_REQUIRED_COMPONENTS_MACRO # SGEXT does not provide components
  INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  )

# Install tree
set(SGEXT_TARGETS_FILE ${CMAKE_INSTALL_PREFIX}/lib/sgext/cmake/SGEXTTargets.cmake)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SGEXTConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/SGEXTConfig.cmake
  INSTALL_DESTINATION lib/sgext/cmake
  PATH_VARS SGEXT_TARGETS_FILE
  NO_CHECK_REQUIRED_COMPONENTS_MACRO # SGEXT does not provide components
  )

