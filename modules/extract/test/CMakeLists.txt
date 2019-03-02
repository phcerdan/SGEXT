set(SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
  ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  ${DGTAL_LIBRARIES}
  ${GTEST_LIBRARIES})
set(SG_MODULE_${SG_MODULE_NAME}_TESTS
  test_merge_nodes.cpp
  test_spatial_graph_reduction.cpp
  test_split_loop.cpp
  )

SG_add_gtests()
