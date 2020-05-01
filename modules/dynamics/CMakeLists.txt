set(SG_MODULE_NAME "Dynamics")
set(SG_MODULE_${SG_MODULE_NAME}_LIBRARY "SG${SG_MODULE_NAME}")
set(SG_LIBRARIES ${SG_LIBRARIES} ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PARENT_SCOPE)
set(SG_MODULE_INTERNAL_DEPENDS SGCore)
set(SG_MODULE_${SG_MODULE_NAME}_DEPENDS
  ${SG_MODULE_INTERNAL_DEPENDS}
  )
set(SG_MODULE_${SG_MODULE_NAME}_SOURCES
    dynamics.cpp
    dynamics_graph_glue.cpp
    force_compute.cpp
    integrator.cpp
    particle.cpp
    bond.cpp
    particle_neighbors.cpp
    particle_collection.cpp
    system.cpp
    bond_collection.cpp
    )

# Use VTK if available
if(SG_REQUIRES_VTK)
  list(APPEND SG_MODULE_${SG_MODULE_NAME}_DEPENDS
    ${VTK_LIBRARIES}
    )
  list(APPEND SG_MODULE_${SG_MODULE_NAME}_SOURCES
    write_vtu_file.cpp
    )
endif()

list(TRANSFORM SG_MODULE_${SG_MODULE_NAME}_SOURCES PREPEND "src/")
add_library(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} ${SG_MODULE_${SG_MODULE_NAME}_SOURCES})
target_include_directories(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  )

if(SG_REQUIRES_VTK)
  target_compile_definitions(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PUBLIC SG_USING_VTK)
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
