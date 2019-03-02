set(SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
  ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  ${GTEST_LIBRARIES})
set(SG_MODULE_${SG_MODULE_NAME}_TESTS
  test_get_vtk_points_from_graph.cpp
  test_graph_points_locator.cpp
  )
# Fixture defined in test/fixtures
list(APPEND SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS graph_locator_fixtures)

SG_add_gtests()
