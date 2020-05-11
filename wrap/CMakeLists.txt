# Python wrapping will be one single module with submodules
set(enabled_libs_ SGCore)
set(enabled_include_dirs_)
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
if(SG_MODULE_VISUALIZE)
  # add_subdirectory(submodules/visualize)
  list(APPEND enabled_libs_ SGVisualize)
endif()
if(SG_BUILD_SCRIPTS)
  add_subdirectory(submodules/scripts)
  list(APPEND enabled_libs_ ${DGTAL_LIBRARIES})
  list(APPEND enabled_libs_ ${ITK_LIBRARIES})
  list(APPEND enabled_libs_ Boost::filesystem)
  list(APPEND enabled_include_dirs_ "${CMAKE_SOURCE_DIR}/cpp-scripts/")
  list(APPEND enabled_include_dirs_ "${DGTAL_INCLUDE_DIRS}")
  # Add itk wrapping of Image
  add_subdirectory(itk)
endif()

# Add the pybind module
pybind11_add_module(_sgext
  sgext_init_py.cpp # Where module is added
  ${all_modules_python_sources} # all the collected sources in submodules
  )
target_link_libraries(_sgext PRIVATE ${enabled_libs_})
target_include_directories(_sgext PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}) # For pybind11_common.h
target_include_directories(_sgext PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/submodules) # For #include "core/sgcore_common_py"
target_include_directories(_sgext PRIVATE ${enabled_include_dirs_})
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
if(SG_BUILD_SCRIPTS)
  target_compile_definitions(_sgext PRIVATE SG_MODULE_SCRIPTS_ENABLED)
endif()

install(TARGETS _sgext
  DESTINATION ${CMAKE_INSTALL_PYTHONLIBDIR}
  COMPONENT runtime
  )
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

if(SG_BUILD_TESTING)
  add_subdirectory(test)
endif()
