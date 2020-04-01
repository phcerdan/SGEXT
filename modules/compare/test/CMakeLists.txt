# Optional VISUALIZE compile definition (linking SGVisualize)
set(SG_MODULE_${SG_MODULE_NAME}_OPTIONAL_TEST_DEPENDS "")
set(SG_MODULE_${SG_MODULE_NAME}_OPTIONAL_TEST_COMPILE_DEFINITION "")
if(SG_MODULE_VISUALIZE)
  option(SG_MODULE_${SG_MODULE_NAME}_ENABLE_VISUALIZE "Enable visualization in tests of this module" OFF)
  mark_as_advanced(SG_MODULE_${SG_MODULE_NAME}_ENABLE_VISUALIZE)
  if(SG_MODULE_${SG_MODULE_NAME}_ENABLE_VISUALIZE)
    list(APPEND SG_MODULE_${SG_MODULE_NAME}_OPTIONAL_TEST_DEPENDS SGVisualize)
    set(SG_MODULE_${SG_MODULE_NAME}_OPTIONAL_TEST_COMPILE_DEFINITION "VISUALIZE")
  endif()
endif()

set(SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
  ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  ${SG_MODULE_${SG_MODULE_NAME}_OPTIONAL_TEST_DEPENDS}
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
  )

SG_add_gtests(
  # Optional compile_definition (VISUALIZE)
  ${SG_MODULE_${SG_MODULE_NAME}_OPTIONAL_TEST_COMPILE_DEFINITION})
