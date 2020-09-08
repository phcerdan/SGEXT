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

# TODO vtk_module_autoinit is not working here, manually added to test file.
if(FALSE)
if(_modern_VTK_available)
  set(test_names)
  foreach(test_file ${SG_MODULE_${SG_MODULE_NAME}_TESTS})
    string(REGEX REPLACE "\\.[^.]*$" "" test_name "${test_file}")
    list(APPEND test_names ${test_name})
  endforeach()
  message(WARNING "test_names: ${test_names}")
  message(WARNING "VTK_LIBRARIES : ${VTK_LIBRARIES}")
  vtk_module_autoinit(
    TARGETS ${test_names}
    MODULES ${VTK_LIBRARIES})
endif()
endif()
