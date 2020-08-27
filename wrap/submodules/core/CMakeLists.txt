set(module_name_ core)
set(module_path_ ${CMAKE_CURRENT_SOURCE_DIR})
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
  transform_to_physical_point_py.cpp
  )
list(TRANSFORM current_sources_ PREPEND "${module_path_}/")

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  PARENT_SCOPE)
