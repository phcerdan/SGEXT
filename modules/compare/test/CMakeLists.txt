set(SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
  ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  ${GTEST_LIBRARIES})
set(SG_MODULE_${SG_MODULE_NAME}_TESTS
  test_compare_graphs.cpp
  )
# Fixture defined in test/fixtures
list(APPEND SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS graph_locator_fixtures)

SG_add_gtests()
