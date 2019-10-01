set(python_tests_
  test_simulated_annealing_generator.py
  )
get_filename_component(module_name_ ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(test_folder "${CMAKE_CURRENT_SOURCE_DIR}")
# test files should start with "test_"
# unittest functions (in .py) should start with "test_" for discover to work
foreach(python_test ${python_tests_})
  add_test(NAME python||${module_name_}||${python_test}
    COMMAND ${PYTHON_EXECUTABLE}
    -m unittest discover
    -s ${test_folder}
    -p ${python_test}
    )
endforeach()
