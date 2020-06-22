set(current_sources_
  sgcore_init_py.cpp
  array_utilities_py.cpp
  boundary_conditions_py.cpp
  spatial_node_py.cpp
  spatial_edge_py.cpp
  spatial_graph_py.cpp
  spatial_graph_io_py.cpp
  edge_points_utilities_py.cpp
  bounding_box_py.cpp
  filter_spatial_graph_py.cpp
  )
set(module_name_ ${CMAKE_CURRENT_SOURCE_DIR})
list(TRANSFORM current_sources_ PREPEND "${module_name_}/")

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  PARENT_SCOPE)
