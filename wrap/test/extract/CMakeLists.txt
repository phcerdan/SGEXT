set(python_tests_
  test_extract.py
  )

get_filename_component(module_name_ ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(test_folder "${CMAKE_CURRENT_SOURCE_DIR}")
# test files should start with "test_"
# unittest functions (in .py) should start with "test_" for discover to work
foreach(python_test ${python_tests_})
  set(python_test_name_ python||${module_name_}||${python_test})
  add_test(NAME ${python_test_name_}
    COMMAND
    ${PYTHON_EXECUTABLE}
    -m unittest discover
    -s ${test_folder}
    -p ${python_test}
    )
  # Append the build folder where python lib is to PYTHONPATH
  # Allowing ctest -R "a_python_test_name_" -V to work
  set_tests_properties(${python_test_name_} PROPERTIES
    ENVIRONMENT PYTHONPATH=$ENV{PYTHONPATH}:${CMAKE_BUILD_PYTHONLIBDIR})
endforeach()
