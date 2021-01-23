set(current_sources_
  sgitk_init_py.cpp
  itk_image_py.cpp
  itk_image_io_py.cpp
  itk_view_image_py.cpp
  transform_to_physical_point_with_itk_py.cpp
  mask_image_py.cpp
  fill_holes_py.cpp
  resample_image_py.cpp
  voxelize_graph_py.cpp
  morphological_watershed_py.cpp
  segmentation_functions_py.cpp
  )
set(module_name_ ${CMAKE_CURRENT_SOURCE_DIR})
list(TRANSFORM current_sources_ PREPEND "${module_name_}/")

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  PARENT_SCOPE)

set(_pure_python_files
  from_to.py
  )

# Copy pure python at configure time
file(COPY ${_pure_python_files} DESTINATION ${CMAKE_BUILD_PYTHONLIBDIR})
# Handle installation of pure python
install(FILES ${_pure_python_files}
  DESTINATION ${CMAKE_INSTALL_PYTHONLIBDIR}
  COMPONENT runtime
  )
