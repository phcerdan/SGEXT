set(module_lib_ ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY})
string(TOLOWER ${module_lib_} SG_MODULE_PYTHON_NAME)
pybind11_add_module(${SG_MODULE_PYTHON_NAME}
  sggenerate_init_py.cpp
  simulated_annealing_generator_py.cpp
  contour_length_generator_py.cpp
  )
target_link_libraries(${SG_MODULE_PYTHON_NAME} PRIVATE ${module_lib_})

install(TARGETS ${SG_MODULE_PYTHON_NAME}
  DESTINATION ${CMAKE_INSTALL_PYTHONLIBDIR})


if(SG_BUILD_TESTING)
  set(test_folder "${CMAKE_CURRENT_SOURCE_DIR}/test_python")
  set(python_tests_
    test_simulated_annealing_generator.py
  )

  # test files should start with "test_"
  # unittest functions (in .py) should start with "test_" for discover to work
  foreach(python_test ${python_tests_})
      add_test(NAME python||${SG_MODULE_PYTHON_NAME}||${python_test}
        COMMAND ${PYTHON_EXECUTABLE}
          -m unittest discover
          -s ${test_folder}
          -p ${python_test}
      )
  endforeach()
endif()
