include(GoogleTest)
# set(TEST_LIBRARIES SG ${Boost_LIBRARIES} ${DGTAL_LIBRARIES} Qt5::Widgets )
set(GTEST_LIBRARIES gtest gtest_main gmock)
set(TEST_LIBRARIES SG ${Boost_LIBRARIES} ${DGTAL_LIBRARIES} ${GTEST_LIBRARIES})

add_executable(test_bounding_box test_bounding_box.cpp)
target_link_libraries(test_bounding_box ${Boost_LIBRARIES} ${GTEST_LIBRARIES})
gtest_discover_tests(
  test_bounding_box
  TEST_PREFIX bounding_box||
  )

add_executable(test_filter_spatial_graph test_filter_spatial_graph.cpp)
target_link_libraries(test_filter_spatial_graph SG ${Boost_LIBRARIES} ${GTEST_LIBRARIES})
gtest_discover_tests(
  test_filter_spatial_graph
  TEST_PREFIX filter_spatial_graph||
  )

add_executable(test_transform_to_physical_point test_transform_to_physical_point.cpp)
target_link_libraries(test_transform_to_physical_point ${ITK_LIBRARIES} ${GTEST_LIBRARIES})
gtest_discover_tests(
  test_transform_to_physical_point
  TEST_PREFIX transform_to_physical_point||
  )

add_executable(test_trim_graph test_trim_graph.cpp)
target_link_libraries(test_trim_graph SG ${GTEST_LIBRARIES})
gtest_discover_tests(
  test_trim_graph
  TEST_PREFIX trim_graph||
  )

add_executable(test_split_loop test_split_loop.cpp)
target_link_libraries(test_split_loop ${TEST_LIBRARIES} ${ITK_LIBRARIES})
gtest_discover_tests(
  test_split_loop
  TEST_PREFIX split_loop||
  )

add_executable(test_spatial_graph_reduction test_spatial_graph_reduction.cpp)
target_link_libraries(test_spatial_graph_reduction ${TEST_LIBRARIES} ${ITK_LIBRARIES})
gtest_discover_tests(
  test_spatial_graph_reduction
  TEST_PREFIX spatial_graph_reduction||
  )

add_executable(test_merge_nodes test_merge_nodes.cpp)
target_link_libraries(test_merge_nodes ${TEST_LIBRARIES} ${ITK_LIBRARIES})
gtest_discover_tests(
  test_merge_nodes
  TEST_PREFIX merge_nodes||
  )

add_executable(test_edge_points_utilities test_edge_points_utilities.cpp)
target_link_libraries(test_edge_points_utilities ${TEST_LIBRARIES})
gtest_discover_tests(
  test_edge_points_utilities
  TEST_PREFIX edge_points_utilities||
  )

add_executable(test_graphviz_io test_graphviz_io.cpp)
target_link_libraries(test_graphviz_io ${TEST_LIBRARIES} ${ITK_LIBRARIES})
gtest_discover_tests(
  test_graphviz_io
  TEST_PREFIX graphviz_io||
  )

add_executable(test_compute_graph_properties test_compute_graph_properties.cpp)
target_link_libraries(test_compute_graph_properties ${TEST_LIBRARIES} ${ITK_LIBRARIES})
gtest_discover_tests(
  test_compute_graph_properties
  TEST_PREFIX compute_graph_properties||
  )

add_executable(test_spatial_histograms test_spatial_histograms.cpp)
target_link_libraries(test_spatial_histograms ${TEST_LIBRARIES} ${ITK_LIBRARIES})
gtest_discover_tests(
  test_spatial_histograms
  TEST_PREFIX spatial_histograms||
  )

add_executable(test_graph_data test_graph_data.cpp)
target_link_libraries(test_graph_data SG ${GTEST_LIBRARIES})
gtest_discover_tests(
  test_graph_data
  TEST_PREFIX graph_data||
  )
#
if(VISUALIZE)
  add_executable(test_visualize_spatial_graph test_visualize_spatial_graph.cpp)
  target_link_libraries(test_visualize_spatial_graph
    SGVtk
    ${ITK_LIBRARIES} # ITKVtkGlue is bringing VTK
    ${GTEST_LIBRARIES}
    )
  gtest_discover_tests(
    test_visualize_spatial_graph
    TEST_PREFIX visualize_spatial_graph||
    )

  add_executable(test_visualize_histograms test_visualize_histograms.cpp
    ${CMAKE_SOURCE_DIR}/src/visualize_histograms.cpp )
  target_link_libraries(test_visualize_histograms
    SG
    SGVtk
    ${GTEST_LIBRARIES}
    )
  gtest_discover_tests(
    test_visualize_histograms
    TEST_PREFIX visualize_histograms||
    )

  add_executable(test_get_vtk_points_from_graph test_get_vtk_points_from_graph.cpp
    ${CMAKE_SOURCE_DIR}/src/get_vtk_points_from_graph.cpp )
  target_link_libraries(test_get_vtk_points_from_graph
    SG
    SGVtk
    ${GTEST_LIBRARIES}
    )
  gtest_discover_tests(
    test_get_vtk_points_from_graph
    TEST_PREFIX get_vtk_points_from_graph||
    )

  add_executable(test_graph_points_locator test_graph_points_locator.cpp
    ${CMAKE_SOURCE_DIR}/src/graph_points_locator.cpp )
  target_link_libraries(test_graph_points_locator
    SG
    SGVtk
    ${GTEST_LIBRARIES}
    )
  gtest_discover_tests(
    test_graph_points_locator
    TEST_PREFIX graph_points_locator||
    )
endif()
