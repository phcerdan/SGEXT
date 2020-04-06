set(SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
  ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  ${GTEST_LIBRARIES})

set(SG_MODULE_${SG_MODULE_NAME}_TESTS
  test_bounding_box.cpp
  test_edge_points_utilities.cpp
  test_filter_spatial_graph.cpp
  test_graph_data.cpp
  test_graphviz_io.cpp
  test_shortest_path.cpp
  test_split_edge.cpp
  test_boundary_conditions.cpp
  test_spatial_graph_utilities.cpp
  )
if(SG_REQUIRES_ITK)
  list(APPEND SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS ${ITK_LIBRARIES})
  list(APPEND SG_MODULE_${SG_MODULE_NAME}_TESTS test_transform_to_physical_point.cpp)
endif()

SG_add_gtests()
