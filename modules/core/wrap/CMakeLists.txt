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
  # test files should start with "test_"
  # unittest functions (in .py) should start with "test_" for discover to work
  add_test(NAME python-test-${SG_MODULE_PYTHON_NAME}
    COMMAND ${PYTHON_EXECUTABLE}
      -m unittest discover
      -s ${CMAKE_CURRENT_SOURCE_DIR}/test_python
  )
endif()
