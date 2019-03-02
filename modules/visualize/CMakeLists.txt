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
    histo)
set(qt_sources "")
if(SG_MODULE_VISUALIZE_ENABLE_QT)
    set(qt_sources visualize_histograms.cpp)
endif()
set(SG_MODULE_${SG_MODULE_NAME}_SOURCES
    ${qt_sources}
    convert_to_vtk_graph.cpp
    visualize_spatial_graph.cpp
    visualize_spatial_graph_with_image.cpp
    )
list(TRANSFORM SG_MODULE_${SG_MODULE_NAME}_SOURCES PREPEND "src/")
add_library(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} ${SG_MODULE_${SG_MODULE_NAME}_SOURCES})
target_include_directories(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )
target_include_directories(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PUBLIC
  ${VTK_INCLUDE_DIRS}
  ${ITK_INCLUDE_DIRS}
  )
target_link_libraries(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  )
add_dependencies(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} ${SG_MODULE_INTERNAL_DEPENDS})

if(SG_BUILD_TESTING)
  add_subdirectory(test)
endif()

install(TARGETS ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
        EXPORT SGEXTTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION include/sgext
  PATTERN "*.txt" EXCLUDE)

# export to the build tree
export( TARGETS ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  NAMESPACE SGEXT::
  APPEND FILE ${sgext_export_file})
