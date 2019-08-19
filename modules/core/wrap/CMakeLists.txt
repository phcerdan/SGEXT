set(module_lib_ ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY})
string(TOLOWER ${module_lib_} SG_MODULE_PYTHON_NAME)
pybind11_add_module(${SG_MODULE_PYTHON_NAME}
  sgcore_init_py.cpp
  array_utilities_py.cpp
  spatial_node_py.cpp
  spatial_edge_py.cpp
  spatial_graph_py.cpp
  spatial_graph_io_py.cpp
  edge_points_utilities_py.cpp
  )
target_link_libraries(${SG_MODULE_PYTHON_NAME} PRIVATE ${module_lib_})

install(TARGETS ${SG_MODULE_PYTHON_NAME}
  DESTINATION ${CMAKE_INSTALL_PYTHONLIBDIR})


if(SG_BUILD_TESTING)
  set(test_folder "${CMAKE_CURRENT_SOURCE_DIR}/test_python")
  set(python_tests_
    test_array3d.py
    test_edge_points_utilities.py
    test_graph.py
    test_spatial_graph_io.py
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
