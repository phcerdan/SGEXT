find_package(TBB REQUIRED) # For c++17 std::execution::par_unseq
set(SG_MODULE_NAME "Generate")
set(SG_MODULE_${SG_MODULE_NAME}_LIBRARY "SG${SG_MODULE_NAME}")
list(APPEND SG_LIBRARIES ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY})
set(SG_MODULE_INTERNAL_DEPENDS SGCore)
set(SG_MODULE_${SG_MODULE_NAME}_DEPENDS
  ${SG_MODULE_INTERNAL_DEPENDS}
  ${TBB_LIBRARIES}
  ${Boost_LIBRARIES}
  histo)
set(SG_MODULE_${SG_MODULE_NAME}_SOURCES
    generate_common.cpp
    simulated_annealing_generator.cpp
    simulated_annealing_generator_config_tree.cpp
    update_step.cpp
    update_step_move_node.cpp
    update_step_swap_edges.cpp
    degree_viger_generator.cpp
    degree_sequences.cpp
    )
list(TRANSFORM SG_MODULE_${SG_MODULE_NAME}_SOURCES PREPEND "src/")
add_library(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} ${SG_MODULE_${SG_MODULE_NAME}_SOURCES})
target_include_directories(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
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

if(SG_BUILD_SCRIPTS)
  add_subdirectory(bin)
endif()