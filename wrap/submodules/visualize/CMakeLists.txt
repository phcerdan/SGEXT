set(current_sources_
  sgvisualize_init_py.cpp
  convert_to_vtk_unstructured_grid_py.cpp
  )
set(module_name_ ${CMAKE_CURRENT_SOURCE_DIR})
list(TRANSFORM current_sources_ PREPEND "${module_name_}/")

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  PARENT_SCOPE)
