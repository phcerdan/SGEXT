cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0022 NEW)

project(histo)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HISTO_HEADERS
    ${INCLUDE_DIR}/histo.hpp
    ${INCLUDE_DIR}/visualize_histo.hpp
    )
# Interface library for header only.
add_library(histo INTERFACE)
target_include_directories(histo INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>  # <prefix>/include/mylib
    )
file(COPY ${HISTO_HEADERS} DESTINATION include)
install(FILES ${HISTO_HEADERS} DESTINATION include)

option(HISTO_WITH_VTK "Use vtk to visualize histogram" OFF)
if(HISTO_WITH_VTK)
    set(_modern_VTK_available FALSE)
    set(_vtk_prefix "vtk")
    find_package(VTK REQUIRED)
    if(VTK_VERSION VERSION_GREATER_EQUAL 8.90.0)
        set(_modern_VTK_available TRUE)
        set(_vtk_prefix "")
    endif()
    set(vtk_components
        ${_vtk_prefix}ChartsCore
        ${_vtk_prefix}CommonCore
        ${_vtk_prefix}CommonDataModel
        ${_vtk_prefix}InteractionStyle
        ${_vtk_prefix}RenderingOpenGL2 # avoid vtkRenderer, vtkRenderWindow not found at run time
        ${_vtk_prefix}RenderingContextOpenGL2 # avoid vtkContextDevice2D not found at run time
        ${_vtk_prefix}ViewsContext2D
         )
    find_package(VTK REQUIRED COMPONENTS
        ${vtk_components}
        CONFIG)
    if(NOT _modern_VTK_available)
        include(${VTK_USE_FILE})
    endif()
    target_link_libraries(histo INTERFACE ${VTK_LIBRARIES})
endif()


set(ENABLE_GOOGLE_TEST "OFF" CACHE BOOL "Requires Gtest installed")

if(ENABLE_GOOGLE_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
