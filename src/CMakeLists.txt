cmake_minimum_required(VERSION 3.5)
# cmake_policy(SET CMP0012 NEW)
# if (UNIX)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -pedantic -Wextra")
# endif()
# cmake ../src  -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE
project(dgtal-play)
option(ENABLE_TESTING "Enable Tests" OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS
        program_options
        filesystem
        graph
        REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
find_package(DGtal REQUIRED)
include_directories(SYSTEM ${DGTAL_INCLUDE_DIRS})
# find_package(ITK REQUIRED)
find_package(ITK REQUIRED COMPONENTS
  ITKCommon
  ITKIOImageBase
  ITKImageGrid
  ITKImageIntensity
  ITKImageStatistics
  ITKVtkGlue
  IsotropicWavelets
  ITKImageIO
  CONFIG
)
include(${ITK_USE_FILE})
include_directories(SYSTEM ${ITK_INCLUDE_DIRS})
find_package(Qt5 REQUIRED
    Widgets
    Xml
    OpenGL)

# find_package(VTK REQUIRED)
find_package(VTK REQUIRED COMPONENTS
  vtkCommonCore
  vtkCommonDataModel
  vtkInteractionWidgets
  vtkRenderingCore
  vtkViewsInfovis
  CONFIG
)
# USE_FILE is deprecated? This is the cause of AUTOINIT warnings
# https://gitlab.kitware.com/vtk/vtk/issues/15895
# include(${VTK_USE_FILE})
include_directories(SYSTEM ${VTK_INCLUDE_DIRS})

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
    ${SG_INCLUDE_DIR}/histo.hpp
    #}
    # Taken from your phcerdan/ITKIsotropicWavelets module. {
    ${SG_INCLUDE_DIR}/itkViewImage.h
    ${SG_INCLUDE_DIR}/itkViewImage.hxx
    # }
    )

set(SG_SRC_WITH_BOOST
    ${SG_SRC_DIR}/trim_graph.cpp
    ${SG_SRC_DIR}/merge_nodes.cpp
    ${SG_SRC_DIR}/remove_extra_edges.cpp
    ${SG_SRC_DIR}/compute_graph_properties.cpp
    ${SG_SRC_DIR}/spatial_histograms.cpp
    ${SG_SRC_DIR}/split_loop.cpp
    ${SG_SRC_DIR}/reduce_spatial_graph_via_dfs.cpp
    )
set(SG_SRC_WITH_VTK
    ${SG_SRC_DIR}/convert_to_vtk_graph.cpp
    ${SG_SRC_DIR}/visualize_spatial_graph.cpp
    ${SG_SRC_DIR}/visualize_spatial_graph_with_image.cpp
    )

add_library(SG ${SG_SRC_WITH_BOOST})
target_include_directories(SG PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/mylib
  )
target_link_libraries(SG ${Boost_LIBRARIES} )

add_library(SGVtk ${SG_SRC_WITH_VTK})
target_include_directories(SGVtk PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/mylib
  )
target_link_libraries(SGVtk ${Boost_LIBRARIES} ${VTK_LIBRARIES} )

add_executable(thin thin.cpp)
target_link_libraries(thin ${Boost_LIBRARIES} )
target_link_libraries(thin ${ITK_LIBRARIES} )
target_link_libraries(thin ${DGTAL_LIBRARIES} )
target_link_libraries(thin Qt5::Widgets Qt5::Xml )
target_link_libraries(thin ${VTK_LIBRARIES} )

add_executable(analyze_graph analyze_graph.cpp)
target_link_libraries(analyze_graph SG )
target_link_libraries(analyze_graph SGVtk )
target_link_libraries(analyze_graph ${Boost_LIBRARIES} )
target_link_libraries(analyze_graph ${ITK_LIBRARIES} )
target_link_libraries(analyze_graph ${DGTAL_LIBRARIES} )
target_link_libraries(analyze_graph Qt5::Widgets Qt5::Xml )
# target_link_libraries(analyze_graph ${VTK_LIBRARIES} )

add_executable(highlight_image_points highlight_image_points.cpp)
target_link_libraries(highlight_image_points ${Boost_LIBRARIES} )
target_link_libraries(highlight_image_points ${ITK_LIBRARIES} )
target_link_libraries(highlight_image_points ${DGTAL_LIBRARIES} )
target_link_libraries(highlight_image_points Qt5::Widgets Qt5::Xml )
target_link_libraries(highlight_image_points ${VTK_LIBRARIES} )

if(ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif(ENABLE_TESTING)
