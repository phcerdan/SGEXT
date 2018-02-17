set(TEST_LIBRARIES ${Boost_LIBRARIES} ${DGTAL_LIBRARIES} Qt5::Widgets Qt5::Xml ${VTK_LIBRARIES})
add_executable(test_spatial_graph_reduction test_spatial_graph_reduction.cpp)
target_link_libraries(test_spatial_graph_reduction ${TEST_LIBRARIES} ${ITK_LIBRARIES})

add_test(NAME spatial_graph_reduction COMMAND test_spatial_graph_reduction)

add_executable(test_visualize_spatial_graph test_visualize_spatial_graph.cpp)
target_link_libraries(test_visualize_spatial_graph ${TEST_LIBRARIES} ${ITK_LIBRARIES})
add_test(NAME visualize_spatial_graph COMMAND test_visualize_spatial_graph)

add_executable(test_graphviz_io test_graphviz_io.cpp)
target_link_libraries(test_graphviz_io ${TEST_LIBRARIES} ${ITK_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME graphviz_io COMMAND test_graphviz_io)

