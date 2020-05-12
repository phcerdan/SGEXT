set(SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
  ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  ${GTEST_LIBRARIES})
set(_with_qt_tests "")
if(SG_MODULE_VISUALIZE_WITH_QT)
    set(_with_qt_tests test_visualize_histograms.cpp)
endif()
if(SG_MODULE_LOCATE)
  set(_with_locate_tests test_visualize_spatial_graph_with_points.cpp)
  list(APPEND SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS SGLocate)
endif()
set(SG_MODULE_${SG_MODULE_NAME}_TESTS
  test_visualize_spatial_graph.cpp
  test_convert_to_vtk_unstructured_grid.cpp
  ${_with_qt_tests}
  ${_with_locate_tests}
  )

SG_add_gtests()
