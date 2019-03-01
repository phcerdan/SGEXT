# Add gtest fixtures as libraries

add_library(graph_locator_fixtures graph_locator_fixtures.cpp)
target_include_directories(graph_locator_fixtures PUBLIC .)
target_link_libraries(graph_locator_fixtures
  SGCore
  ${Boost_LIBRARIES}
  ${GTEST_LIBRARIES}
  )
