set(module_name_ compare)
set(module_path_ ${CMAKE_CURRENT_SOURCE_DIR})
set(current_sources_
  sgcompare_init_py.cpp
  extend_low_info_graph_py.cpp
  add_graph_peninsulas_py.cpp
  spatial_graph_difference_py.cpp
  )
list(TRANSFORM current_sources_ PREPEND "${module_path_}/")

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  PARENT_SCOPE)
