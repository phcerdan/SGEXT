set(SG_MODULE_NAME "Visualize")
set(SG_MODULE_${SG_MODULE_NAME}_LIBRARY "SG${SG_MODULE_NAME}")
set(SG_LIBRARIES ${SG_LIBRARIES} ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PARENT_SCOPE)
set(SG_MODULE_INTERNAL_DEPENDS SGCore)
set(visualize_optional_depends "")
if(SG_REQUIRES_ITK)
  list(APPEND visualize_optional_depends ${ITK_LIBRARIES})
endif()
if(SG_MODULE_VISUALIZE_WITH_QT)
  list(APPEND visualize_optional_depends Qt5::Widgets)
endif()
set(SG_MODULE_${SG_MODULE_NAME}_DEPENDS
    ${SG_MODULE_INTERNAL_DEPENDS}
    ${visualize_optional_depends}
    ${VTK_LIBRARIES}
    )
set(qt_sources "")
if(SG_MODULE_VISUALIZE_WITH_QT)
    set(qt_sources visualize_histograms.cpp)
endif()

set(modern_vtk_sources)
if(SG_REQUIRES_ITK AND _modern_VTK_available)
  set(modern_vtk_sources
    # Requires OpenGL2
    visualize_spatial_graph_with_image.cpp
    )
endif()

set(SG_MODULE_${SG_MODULE_NAME}_SOURCES
    ${qt_sources}
    convert_to_vtk_graph.cpp
    convert_to_vtk_unstructured_grid.cpp
    visualize_common.cpp
    visualize_object.cpp
    visualize_spatial_graph.cpp
    vtkInteractorStyleTrackballCameraGraph.cpp
    ${modern_vtk_sources}
    )
list(TRANSFORM SG_MODULE_${SG_MODULE_NAME}_SOURCES PREPEND "src/")
add_library(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} ${SG_MODULE_${SG_MODULE_NAME}_SOURCES})
target_include_directories(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_include_directories(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PUBLIC
  ${VTK_INCLUDE_DIRS}
  ${DGTAL_INCLUDE_DIRS}
  )
if(SG_REQUIRES_ITK)
  target_include_directories(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PUBLIC
    ${ITK_INCLUDE_DIRS}
    )
endif()
target_link_libraries(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  PUBLIC
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  )
target_link_libraries(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  PUBLIC
  ${SG_MODULE_INTERNAL_DEPENDS}
  )

if(SG_BUILD_TESTING)
  add_subdirectory(test)
endif()

install(TARGETS ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
        EXPORT SGEXTTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT runtime
        )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sgext
  PATTERN "*.txt" EXCLUDE)

# export to the build tree
export( TARGETS ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  NAMESPACE SGEXT::
  APPEND FILE ${sgext_export_file})
