set(module_name_ tree)
set(module_path_ ${CMAKE_CURRENT_SOURCE_DIR})
set(current_sources_
  sgtree_init_py.cpp
  create_vertex_to_radius_map_py.cpp
  tree_generation_py.cpp
  )
list(TRANSFORM current_sources_ PREPEND "${module_path_}/")

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  PARENT_SCOPE)
