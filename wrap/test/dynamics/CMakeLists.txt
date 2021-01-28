set(python_tests
  test_particle.py
  test_bond.py
  test_particle_collection.py
  test_bond_collection.py
  test_system.py
  test_force_compute.py
  test_integrator.py
  )

get_filename_component(module_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(test_folder "${CMAKE_CURRENT_SOURCE_DIR}")

SG_add_python_tests()
