pybind11_add_module(sgcore
  sgcore_init.cpp
  array_utilities_pybind.cpp
  spatial_node_pybind.cpp
  spatial_edge_pybind.cpp
  spatial_graph_pybind.cpp
  )
target_link_libraries(sgcore PRIVATE SGCore)
