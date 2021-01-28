# Minimum requirement is a list of cpp:
# set(SG_MODULE_${SG_MODULE_NAME}_TESTS
#  test_get_vtk_points_from_graph.cpp
#  test_graph_points_locator.cpp
# )
# And the dependencies
#  set(SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
#    ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
#    ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
#    ${GTEST_LIBRARIES})
# It accepts an optional argument with compile definitions for all the tests listed in the module, i.e VISUALIZE
# SG_add_gtests(${OPTIONAL_COMPILE_DEFINITIONS})
macro(SG_add_gtests)
  foreach(test_file ${SG_MODULE_${SG_MODULE_NAME}_TESTS})
    string(REGEX REPLACE "\\.[^.]*$" "" test_name "${test_file}")
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE
      ${SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS}
      )
    target_include_directories(${test_name} SYSTEM PRIVATE
      ${SG_MODULE_${SG_MODULE_NAME}_TEST_SYSTEM_INCLUDE_DIRS})
    gtest_discover_tests(
      ${test_name}
      TEST_PREFIX ${SG_MODULE_NAME}__${test_name}__
      PROPERTIES LABELS ${SG_MODULE_NAME}
      )
    if(${ARGC} GREATER 2)
      target_compile_definitions(${test_name} PRIVATE ${ARGV1})
    endif()
  endforeach()
endmacro(SG_add_gtests)

# Minimum requirement is a list of python_tests_:
#  set(python_tests_
#   test_array3d.py
#   test_graph.py
#  )
# And the module_name_ and test_folder:
#  get_filename_component(module_name_ ${CMAKE_CURRENT_SOURCE_DIR} NAME)
#  set(test_folder "${CMAKE_CURRENT_SOURCE_DIR}")
macro(SG_add_python_tests)
  # test files should start with "test_"
  # unittest functions (in .py) should start with "test_" for discover to work
  foreach(python_test ${python_tests})
    set(python_test_name_ python||${module_name}||${python_test})
    add_test(NAME ${python_test_name_}
      COMMAND
      ${PYTHON_EXECUTABLE}
      -m unittest discover
      -s ${test_folder}
      -p ${python_test}
      )
    # Append the build folder where python lib is to PYTHONPATH
    # Allowing ctest -R "a_python_test_name_" -V to work
    if(WIN32)
      set(_path_sep "\;")
    else()
      set(_path_sep ":")
    endif()
    set_tests_properties(${python_test_name_} PROPERTIES
      ENVIRONMENT "PYTHONPATH=$ENV{PYTHONPATH}${_path_sep}${CMAKE_BUILD_PYTHONLIBDIR}/..")
  endforeach()
endmacro(SG_add_python_tests)
