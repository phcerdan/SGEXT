set(TEST_LIBRARIES ${Boost_LIBRARIES} ${DGTAL_LIBRARIES} Qt5::Widgets Qt5::Xml)
add_executable(test_spatial_graph_reduction test_spatial_graph_reduction.cpp)
target_link_libraries(test_spatial_graph_reduction ${TEST_LIBRARIES} ${ITK_LIBRARIES})

add_test(NAME spatial_graph_reduction COMMAND test_spatial_graph_reduction)

add_executable(test_boost_city_visitor test_boost_city_visitor.cpp)
target_link_libraries(test_boost_city_visitor ${TEST_LIBRARIES} ${ITK_LIBRARIES})
add_test(NAME boost_city_visitor COMMAND test_boost_city_visitor)
