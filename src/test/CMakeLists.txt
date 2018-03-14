set(TEST_LIBRARIES SG ${Boost_LIBRARIES} ${DGTAL_LIBRARIES} Qt5::Widgets Qt5::Xml)

add_executable(test_trim_graph test_trim_graph.cpp)
target_link_libraries(test_trim_graph SG)
add_test(NAME trim_graph COMMAND test_trim_graph)

add_executable(test_visualize_spatial_graph test_visualize_spatial_graph.cpp)
target_link_libraries(test_visualize_spatial_graph
  SGVtk
  ${ITK_LIBRARIES} # ITKVtkGlue is bringing VTK
  )
add_test(NAME visualize_spatial_graph COMMAND test_visualize_spatial_graph)

add_executable(test_spatial_graph_reduction test_spatial_graph_reduction.cpp)
target_link_libraries(test_spatial_graph_reduction ${TEST_LIBRARIES} ${ITK_LIBRARIES})
add_test(NAME spatial_graph_reduction COMMAND test_spatial_graph_reduction)

add_executable(test_merge_nodes test_merge_nodes.cpp)
target_link_libraries(test_merge_nodes ${TEST_LIBRARIES} ${ITK_LIBRARIES})
add_test(NAME merge_nodes COMMAND test_merge_nodes)


add_executable(test_graphviz_io test_graphviz_io.cpp)
target_link_libraries(test_graphviz_io ${TEST_LIBRARIES} ${ITK_LIBRARIES})
add_test(NAME graphviz_io COMMAND test_graphviz_io)

add_executable(test_compute_graph_properties test_compute_graph_properties.cpp)
target_link_libraries(test_compute_graph_properties ${TEST_LIBRARIES} ${ITK_LIBRARIES})
add_test(NAME compute_graph_properties COMMAND test_compute_graph_properties)

add_executable(test_spatial_histograms test_spatial_histograms.cpp)
target_link_libraries(test_spatial_histograms ${TEST_LIBRARIES} ${ITK_LIBRARIES})
add_test(NAME spatial_histograms COMMAND test_spatial_histograms)
