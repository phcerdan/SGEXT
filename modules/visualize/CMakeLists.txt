set(SG_MODULE_NAME "Visualize")
set(SG_MODULE_${SG_MODULE_NAME}_LIBRARY "SG${SG_MODULE_NAME}")
list(APPEND SG_LIBRARIES ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY})
set(SG_MODULE_INTERNAL_DEPENDS SGCore)
set(qt_targets "")
if(SG_MODULE_VISUALIZE_ENABLE_QT)
    set(qt_targets Qt5::Widgets)
endif()
set(SG_MODULE_${SG_MODULE_NAME}_DEPENDS
    ${SG_MODULE_INTERNAL_DEPENDS}
    ${ITK_LIBRARIES}
    ${qt_targets}
    ${VTK_LIBRARIES}
    ${DGTAL_LIBRARIES} # For visualize_object.hpp
    histo)
set(qt_sources "")
if(SG_MODULE_VISUALIZE_ENABLE_QT)
    set(qt_sources visualize_histograms.cpp)
endif()

set(modern_vtk_sources)
if(${_modern_VTK_available})
  set(modern_vtk_sources
    # Requires OpenGL2
    visualize_spatial_graph_with_image.cpp
    )
endif()

set(SG_MODULE_${SG_MODULE_NAME}_SOURCES
    ${qt_sources}
    convert_to_vtk_graph.cpp
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
  ${ITK_INCLUDE_DIRS}
  ${DGTAL_INCLUDE_DIRS}
  )
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
        )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sgext
  PATTERN "*.txt" EXCLUDE)

# export to the build tree
export( TARGETS ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  NAMESPACE SGEXT::
  APPEND FILE ${sgext_export_file})
