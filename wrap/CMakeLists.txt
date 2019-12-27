# Python wrapping will be one single module with submodules
set(enabled_libs_ SGCore)
add_subdirectory(submodules/core)
# add_subdirectory(submodules/extract) # not implemented yet
if(SG_MODULE_GENERATE)
  add_subdirectory(submodules/generate)
  list(APPEND enabled_libs_ SGGenerate)
endif()
if(SG_MODULE_DYNAMICS)
  add_subdirectory(submodules/dynamics)
  list(APPEND enabled_libs_ SGDynamics)
endif()

# Add the pybind module
pybind11_add_module(_sgext
  sgext_init_py.cpp # Where module is added
  ${all_modules_python_sources} # all the collected sources in submodules
  )
target_link_libraries(_sgext PRIVATE ${enabled_libs_})
if(SG_REQUIRES_VTK)
  target_compile_definitions(_sgext PRIVATE SG_USING_VTK)
endif()

install(TARGETS _sgext
  DESTINATION ${CMAKE_INSTALL_PYTHONLIBDIR})
install(FILES __init__.py
  DESTINATION ${CMAKE_INSTALL_PYTHONLIBDIR})

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

if(SG_BUILD_TESTING)
  add_subdirectory(test)
endif()
