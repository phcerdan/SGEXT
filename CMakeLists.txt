cmake_minimum_required(VERSION 3.8.1)

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

project(SGEXT
  VERSION 0.8.0
  LANGUAGES CXX)
message(STATUS "SGEXT version: ${SGEXT_VERSION}")

option(ENABLE_TESTING "Enable Tests" OFF)
option(VISUALIZE "Enable visualization with VTK and QT." OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS
        program_options
        filesystem
        graph
        serialization
        REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
# Requires version with this PR in: https://github.com/DGtal-team/DGtal/pull/1369
# Merged in DGtal 1.0.beta
find_package(DGtal REQUIRED 1.0)
include_directories(SYSTEM ${DGTAL_INCLUDE_DIRS})

find_package(ITK CONFIG REQUIRED)
include(${ITK_USE_FILE})

set(ITKImageIOModule "")
if(${ITK_VERSION} VERSION_LESS "4.13.0")
# Create list of avaialble image formats
foreach(ImageFormat ${LIST_OF_IMAGEIO_FORMATS})
  if (NOT ${ImageFormat}_image_module_name )
    list(APPEND ITKImageIOModule ITKIO${ImageFormat})
  endif()
endforeach()
else()
  set(ITKImageIOModule ITKImageIO)
endif()

set(ITKVtkGlueModule "")
if(VISUALIZE)
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
include(${ITK_USE_FILE})
include_directories(SYSTEM ${ITK_INCLUDE_DIRS})
if(VISUALIZE)
  find_package(Qt5 REQUIRED
    Widgets
    Xml
    OpenGL)

  if (Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()

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
    vtkGUISupportQt # QVTKWidget.h
    CONFIG
    )
  # USE_FILE is deprecated? This is the cause of AUTOINIT warnings
  # https://gitlab.kitware.com/vtk/vtk/issues/15895
  include(${VTK_USE_FILE})
  include_directories(SYSTEM ${VTK_INCLUDE_DIRS})
endif()

set(SG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
include_directories(${SG_INCLUDE_DIR})

set(SG_SRC_DIR "${CMAKE_SOURCE_DIR}/src")
# To keep track of headers, unused elsewhere
set(SG_HEADERS_ONLY
    ${SG_INCLUDE_DIR}/reduce_dfs_visitor.hpp
    ${SG_INCLUDE_DIR}/visualize_spatial_graph_with_image.hpp
    ${SG_INCLUDE_DIR}/spatial_graph_from_object.hpp
    ${SG_INCLUDE_DIR}/spatial_graph_utilities.hpp
    # These could form an external project {{{
    ${SG_INCLUDE_DIR}/array_utilities.hpp
    ${SG_INCLUDE_DIR}/spatial_graph.hpp
    ${SG_INCLUDE_DIR}/spatial_node.hpp
    ${SG_INCLUDE_DIR}/spatial_edge.hpp
    # }}}
    # Already external in phcerdan/histo-header. Use git submodule. {
    # ${SG_INCLUDE_DIR}/histo.hpp
    #}
    # Taken from your phcerdan/ITKIsotropicWavelets module. {
    ${SG_INCLUDE_DIR}/itkViewImage.h
    ${SG_INCLUDE_DIR}/itkViewImage.hxx
    # }
    )

set(HISTO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/histo/include
        CACHE PATH "histo include directory")
install(DIRECTORY ${HISTO_INCLUDE_DIR}/ DESTINATION include/sgext)

# for convenience setup a target
add_library(histo INTERFACE)
target_include_directories(histo INTERFACE
    $<BUILD_INTERFACE:${HISTO_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include/sgext>)

# need to export target as well
install(TARGETS histo EXPORT my_export_set DESTINATION include)
# this exports library: histo

set(SG_SRC_WITH_BOOST
    ${SG_SRC_DIR}/trim_graph.cpp
    ${SG_SRC_DIR}/merge_nodes.cpp
    ${SG_SRC_DIR}/edge_points_utilities.cpp
    ${SG_SRC_DIR}/remove_extra_edges.cpp
    ${SG_SRC_DIR}/compute_graph_properties.cpp
    ${SG_SRC_DIR}/spatial_histograms.cpp
    ${SG_SRC_DIR}/split_loop.cpp
    ${SG_SRC_DIR}/reduce_spatial_graph_via_dfs.cpp
    ${SG_SRC_DIR}/spatial_graph_utilities.cpp
    ${SG_SRC_DIR}/filter_spatial_graph.cpp
    ${SG_SRC_DIR}/serialize_spatial_graph.cpp
    # not requiring boost
    ${SG_SRC_DIR}/graph_data.cpp
    )
set(SG_SRC_WITH_VTK
    ${SG_SRC_DIR}/convert_to_vtk_graph.cpp
    ${SG_SRC_DIR}/visualize_spatial_graph.cpp
    ${SG_SRC_DIR}/visualize_spatial_graph_with_image.cpp
    ${SG_SRC_DIR}/get_vtk_points_from_graph.cpp
    ${SG_SRC_DIR}/graph_points_locator.cpp
    ${SG_SRC_DIR}/compare_graphs.cpp
    )

set(SG_SRC_WITH_VTK_AND_QT
    ${SG_SRC_DIR}/visualize_histograms.cpp
    )

add_library(SG ${SG_SRC_WITH_BOOST})
target_include_directories(SG PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include
  )
target_link_libraries(SG ${Boost_LIBRARIES} histo )

if(VISUALIZE)
  add_library(SGVtk ${SG_SRC_WITH_VTK} )
    target_include_directories(SGVtk PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>  # <prefix>/include/mylib
        )
    target_link_libraries(SGVtk SG )
    target_link_libraries(SGVtk ${Boost_LIBRARIES} ${VTK_LIBRARIES} )
    target_compile_definitions(SGVtk PRIVATE VISUALIZE)
endif()

add_subdirectory(cpp-scripts)

if(ENABLE_TESTING)
  enable_testing()

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

  add_subdirectory(test)
endif(ENABLE_TESTING)


# EXPORT and generate .config file for find_package

set(_target_libraries SG histo)
if(VISUALIZE)
  list(APPEND _target_libraries SGVtk)
endif()
set(sgext_export_file "${PROJECT_BINARY_DIR}/SGEXTTargets.cmake")

install(TARGETS ${_target_libraries}
        EXPORT SGEXTTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )
export( TARGETS ${_target_libraries}
  NAMESPACE SGEXT::
  FILE ${sgext_export_file})

install (EXPORT SGEXTTargets
  NAMESPACE SGEXT::
  DESTINATION lib/sgext/cmake )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION include/sgext
  PATTERN "*.txt" EXCLUDE)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/SGEXTConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/SGEXTConfigVersion.cmake
              DESTINATION lib/sgext/cmake )

include(CMakePackageConfigHelpers)

write_basic_package_version_file (SGEXTConfigVersion.cmake
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

# # Install tree
set(SGEXT_TARGETS_FILE ${CMAKE_INSTALL_PREFIX}/lib/sgext/cmake/SGEXTTargets.cmake)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SGEXTConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/SGEXTConfig.cmake
  INSTALL_DESTINATION lib/sgext/cmake
  PATH_VARS SGEXT_TARGETS_FILE
  NO_CHECK_REQUIRED_COMPONENTS_MACRO # SGEXT does not provide components
  )

