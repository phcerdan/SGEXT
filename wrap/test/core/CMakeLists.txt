set(python_tests
  test_array3d.py
  test_edge_points_utilities.py
  test_graph.py
  test_spatial_graph_io.py
  test_bounding_box.py
  )

get_filename_component(module_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(test_folder "${CMAKE_CURRENT_SOURCE_DIR}")

SG_add_python_tests()
