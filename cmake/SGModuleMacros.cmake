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
      TEST_PREFIX ${SG_MODULE_NAME}||${test_name}||
      PROPERTIES LABELS ${SG_MODULE_NAME}
      )
  endforeach()
endmacro(SG_add_gtests)
