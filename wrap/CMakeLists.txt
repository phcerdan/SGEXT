# Python wrapping will be one single module with submodules
set(CMAKE_INSTALL_PYTHONINCLUDEDIR
  ${CMAKE_INSTALL_PYTHONLIBDIR}/include)
set(enabled_libs_ SGCore)
set(enabled_include_dirs_)
set(enabled_include_system_dirs_)
add_subdirectory(submodules/core)
# MODULE_EXTRACT
add_subdirectory(submodules/extract)
list(APPEND enabled_libs_ SGExtract)
if(SG_MODULE_ANALYZE)
  add_subdirectory(submodules/analyze)
  list(APPEND enabled_libs_ SGAnalyze)
endif()
if(SG_MODULE_GENERATE)
  add_subdirectory(submodules/generate)
  list(APPEND enabled_libs_ SGGenerate)
endif()
if(SG_MODULE_DYNAMICS)
  add_subdirectory(submodules/dynamics)
  list(APPEND enabled_libs_ SGDynamics)
endif()
if(SG_MODULE_LOCATE)
  add_subdirectory(submodules/locate)
  list(APPEND enabled_libs_ SGLocate)
endif()
if(SG_MODULE_VISUALIZE)
  add_subdirectory(submodules/visualize)
  list(APPEND enabled_libs_ SGVisualize)
endif()
if(SG_MODULE_COMPARE)
  add_subdirectory(submodules/compare)
  list(APPEND enabled_libs_ SGCompare)
endif()
if(SG_MODULE_TREE)
  add_subdirectory(submodules/tree)
  list(APPEND enabled_libs_ SGTree)
endif()


if(SG_MODULE_SCRIPTS)
  # Add itk wrapping of Image
  add_subdirectory(itk)

  add_subdirectory(submodules/scripts)
  list(APPEND enabled_libs_ SGScripts)
endif()

set(wrap_header_common
  pybind11_common.h)

# Add the pybind module
# Use SHARED instead of the deault MODULE to allow external plugins to link to it.
# This would require the same CI/CD to build and link these external plugins.
pybind11_add_module(_sgext
  SHARED
  sgext_init_py.cpp # Where module is added
  ${all_modules_python_sources} # all the collected sources in submodules
  )
add_library(SGEXT::_sgext ALIAS _sgext)
target_link_libraries(_sgext PRIVATE ${enabled_libs_})
target_include_directories(_sgext PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PYTHONINCLUDEDIR}>
  ) # For pybind11_common.h
target_include_directories(_sgext PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/submodules>
  ) # For #include "locate/sglocate_common.h"
target_include_directories(_sgext PRIVATE ${enabled_include_dirs_})
target_include_directories(_sgext SYSTEM PRIVATE ${enabled_include_system_dirs_})
if(SG_REQUIRES_VTK)
  target_compile_definitions(_sgext PRIVATE SG_USING_VTK)
endif()
if(SG_MODULE_ANALYZE)
  target_compile_definitions(_sgext PRIVATE SG_MODULE_ANALYZE_ENABLED)
endif()
if(SG_MODULE_GENERATE)
  target_compile_definitions(_sgext PRIVATE SG_MODULE_GENERATE_ENABLED)
endif()
if(SG_MODULE_DYNAMICS)
  target_compile_definitions(_sgext PRIVATE SG_MODULE_DYNAMICS_ENABLED)
endif()
if(SG_MODULE_VISUALIZE)
  target_compile_definitions(_sgext PRIVATE SG_MODULE_VISUALIZE_ENABLED)
  if(SG_MODULE_VISUALIZE_WITH_QT)
    target_compile_definitions(_sgext PRIVATE VISUALIZE)
  endif()
endif()
if(SG_MODULE_LOCATE)
  target_compile_definitions(_sgext PRIVATE SG_MODULE_LOCATE_ENABLED)
endif()
if(SG_MODULE_COMPARE)
  target_compile_definitions(_sgext PRIVATE SG_MODULE_COMPARE_ENABLED)
endif()
if(SG_MODULE_TREE)
  target_compile_definitions(_sgext PRIVATE SG_MODULE_TREE_ENABLED)
endif()
if(SG_MODULE_SCRIPTS)
  target_compile_definitions(_sgext PRIVATE SG_MODULE_SCRIPTS_ENABLED)
endif()

if(_modern_VTK_available)
  vtk_module_autoinit(
    TARGETS _sgext
    MODULES ${VTK_LIBRARIES})
endif()

install(FILES ${wrap_header_common}
  DESTINATION ${CMAKE_INSTALL_PYTHONINCLUDEDIR}
  COMPONENT development)

install(FILES __init__.py
  DESTINATION ${CMAKE_INSTALL_PYTHONLIBDIR}
  COMPONENT runtime
  )
# Copy __init__ at configure time
file(COPY __init__.py DESTINATION ${CMAKE_BUILD_PYTHONLIBDIR})
# Copy at build time
add_custom_command(
  TARGET _sgext
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  $<TARGET_FILE:_sgext>
  ${CMAKE_BUILD_PYTHONLIBDIR}/$<TARGET_FILE_NAME:_sgext>
  )

# Copy and install fixtures
file(COPY fixtures DESTINATION ${CMAKE_BUILD_PYTHONLIBDIR})
install(DIRECTORY fixtures
  DESTINATION ${CMAKE_INSTALL_PYTHONLIBDIR}
  COMPONENT runtime
  )
# Copy and install tables (package_data)
set(table_folder "${PROJECT_SOURCE_DIR}/deploy/sgext/tables")
file(COPY ${table_folder} DESTINATION ${CMAKE_BUILD_PYTHONLIBDIR})
install(DIRECTORY ${table_folder}
  DESTINATION ${CMAKE_INSTALL_PYTHONLIBDIR}
  COMPONENT runtime
  )

# Export _sgext to let external plugins link to SGEXT::_sgext
# export to install tree: (always to CMAKE_INSTALL_PYTHONLIBDIR)
install(TARGETS _sgext
  EXPORT SGEXTTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_PYTHONLIBDIR} COMPONENT runtime
  LIBRARY DESTINATION ${CMAKE_INSTALL_PYTHONLIBDIR} COMPONENT runtime
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PYTHONLIBDIR} COMPONENT development)

# Append to _targets_to_export in the PARENT_SCOPE.
set(_targets_to_export ${_targets_to_export} _sgext PARENT_SCOPE)

if(SG_BUILD_TESTING)
  add_subdirectory(test)
endif()
