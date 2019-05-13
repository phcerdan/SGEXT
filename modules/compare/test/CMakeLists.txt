set(SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
  ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  ${GTEST_LIBRARIES})
set(SG_MODULE_${SG_MODULE_NAME}_TESTS
  test_compare_graphs.cpp
  test_extend_low_info_graph.cpp
  test_spatial_graph_difference.cpp
  )
# Fixture defined in test/fixtures
list(APPEND SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
  FixtureMatchingGraphs
  FixtureSquareCrossGraph
  SGLocate
  SGVisualize
  )

SG_add_gtests()
