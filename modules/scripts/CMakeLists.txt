set(SG_MODULE_NAME "Scripts")
set(SG_MODULE_${SG_MODULE_NAME}_LIBRARY "SG${SG_MODULE_NAME}")
set(SG_LIBRARIES ${SG_LIBRARIES} ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PARENT_SCOPE)

set(enabled_internal_libs_
  SGCore)
set(enabled_include_dirs_)
set(enabled_external_libs_
  ${DGTAL_LIBRARIES}
  ${VTK_LIBRARIES}
  ${ITK_LIBRARIES}
  Boost::filesystem
  )
set(enabled_include_system_dirs_
  ${DGTAL_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  )
set(enabled_compile_definitions_)

list(APPEND enabled_internal_libs_ SGExtract)
if(SG_MODULE_ANALYZE)
  list(APPEND enabled_internal_libs_ SGAnalyze)
endif()
if(SG_MODULE_GENERATE)
  list(APPEND enabled_internal_libs_ SGGenerate)
endif()
if(SG_MODULE_DYNAMICS)
  list(APPEND enabled_internal_libs_ SGDynamics)
endif()
if(SG_MODULE_LOCATE)
  list(APPEND enabled_internal_libs_ SGLocate)
endif()
if(SG_MODULE_VISUALIZE)
  list(APPEND enabled_internal_libs_ SGVisualize)
endif()
if(SG_MODULE_COMPARE)
  list(APPEND enabled_internal_libs_ SGCompare)
endif()

set(SG_MODULE_INTERNAL_DEPENDS ${enabled_internal_libs_})
set(SG_MODULE_${SG_MODULE_NAME}_DEPENDS
  ${SG_MODULE_INTERNAL_DEPENDS}
  ${enabled_external_libs_}
  )

set(SG_MODULE_${SG_MODULE_NAME}_SOURCES
  analyze_graph_function.cpp
  create_distance_map_function.cpp
  fill_holes_function.cpp
  thin_function.cpp
  reconstruct_from_distance_map.cpp
  )

list(TRANSFORM SG_MODULE_${SG_MODULE_NAME}_SOURCES PREPEND "src/")
add_library(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} ${SG_MODULE_${SG_MODULE_NAME}_SOURCES})
add_library(SGEXT::${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} ALIAS ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY})
target_include_directories(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_include_directories(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  SYSTEM
  PUBLIC
  ${enabled_include_system_dirs_}
  )
target_link_libraries(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  )
add_dependencies(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} ${SG_MODULE_INTERNAL_DEPENDS})

if(SG_MODULE_VISUALIZE)
    target_compile_definitions(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
      PRIVATE SG_MODULE_VISUALIZE_ENABLED)
    if(SG_MODULE_VISUALIZE_WITH_QT)
      target_compile_definitions(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
        PRIVATE SG_MODULE_VISUALIZE_ENABLED_WITH_QT)
    endif()
endif()

if(SG_BUILD_TESTING)
  add_subdirectory(test)
endif()

install(TARGETS ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
        EXPORT SGEXTTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT development
        )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sgext
  PATTERN "*.txt" EXCLUDE)

# export to the build tree
export( TARGETS ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  NAMESPACE SGEXT::
  APPEND FILE ${sgext_export_file})
