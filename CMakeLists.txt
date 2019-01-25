cmake_minimum_required(VERSION 3.11)
# cmake_policy(SET CMP0012 NEW)
# if (UNIX)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -pedantic -Wextra")
# endif()
# cmake ../src  -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE
project(object_to_spatial_graph)
option(ENABLE_TESTING "Enable Tests" OFF)
option(VISUALIZE "Enable visualization with VTK and QT." OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS
        program_options
        filesystem
        graph
        REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
# Requires version with this PR in: https://github.com/DGtal-team/DGtal/pull/1369
# Merged in DGtal 1.0.beta
find_package(DGtal REQUIRED 1.0)
include_directories(SYSTEM ${DGTAL_INCLUDE_DIRS})
find_package(ITK REQUIRED COMPONENTS
  ITKCommon
  ITKIOImageBase
  ITKImageGrid
  ITKImageIntensity
  ITKImageStatistics
  ITKVtkGlue
  ITKImageIO
  CONFIG
)
include(${ITK_USE_FILE})
include_directories(SYSTEM ${ITK_INCLUDE_DIRS})
if(VISUALIZE)
  find_package(Qt5 REQUIRED
    Widgets
    Xml
    OpenGL)

  # VTK components to handle external dependency histo-header
  find_package(VTK REQUIRED COMPONENTS
    vtkChartsCore
    vtkCommonCore
    vtkCommonDataModel
    vtkInteractionStyle
    vtkRenderingCore
    vtkRenderingOpenGL2 # avoid vtkRenderer, vtkRenderWindow not found at run time
    vtkRenderingContextOpenGL2 # avoid vtkContextDevice2D not found at run time
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
install(DIRECTORY ${HISTO_INCLUDE_DIR} DESTINATION include})

# for convenience setup a target
add_library(histo INTERFACE)
target_include_directories(histo INTERFACE
    $<BUILD_INTERFACE:${HISTO_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include)

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
    # not requiring boost
    ${SG_SRC_DIR}/graph_data.cpp
    )
set(SG_SRC_WITH_VTK
    ${SG_SRC_DIR}/convert_to_vtk_graph.cpp
    ${SG_SRC_DIR}/visualize_spatial_graph.cpp
    ${SG_SRC_DIR}/visualize_spatial_graph_with_image.cpp
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
    GIT_TAG        release-1.8.0
  )

  FetchContent_GetProperties(googletest)
  if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
  endif()
  #############################################################################

  add_subdirectory(test)
endif(ENABLE_TESTING)
