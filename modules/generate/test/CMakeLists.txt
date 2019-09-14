set(SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
  ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  ${GTEST_LIBRARIES})
set(SG_MODULE_${SG_MODULE_NAME}_TESTS
  test_histograms_in_generate.cpp
  test_simulated_annealing_generator.cpp
  test_update_step_move_node.cpp
  test_update_step_swap_edges.cpp
  test_cramer_von_mises_test.cpp
  test_degree_viger_generator.cpp
  test_contour_length_generator.cpp
  )

SG_add_gtests()
