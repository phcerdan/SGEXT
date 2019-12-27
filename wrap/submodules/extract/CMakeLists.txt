set(current_sources_
  sgextract_init_py.cpp
  merge_nodes_py.cpp
  reduce_spatial_graph_py.cpp
  remove_extra_edges_py.cpp
  split_loop_py.cpp
  )
set(module_name_ ${CMAKE_CURRENT_SOURCE_DIR})
list(TRANSFORM current_sources_ PREPEND "${module_name_}/")

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  PARENT_SCOPE)
