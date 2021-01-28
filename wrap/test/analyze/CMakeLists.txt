set(python_tests
  test_compute_graph_properties.py
  )

get_filename_component(module_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(test_folder "${CMAKE_CURRENT_SOURCE_DIR}")

SG_add_python_tests()
