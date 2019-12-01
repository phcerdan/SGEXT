set(SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
  ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  ${GTEST_LIBRARIES}
  )
set(SG_MODULE_${SG_MODULE_NAME}_TESTS
  test_dynamics.cpp
  test_particle_graph_glue.cpp
  test_integrator.cpp
  )

SG_add_gtests()
