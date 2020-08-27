set(module_name_ analyze)
set(module_path_ ${CMAKE_CURRENT_SOURCE_DIR})
set(current_sources_
  sganalyze_init_py.cpp
  compute_graph_properties_py.cpp
  )
set(module_path_ ${CMAKE_CURRENT_SOURCE_DIR})
list(TRANSFORM current_sources_ PREPEND "${module_path_}/")

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  PARENT_SCOPE)
