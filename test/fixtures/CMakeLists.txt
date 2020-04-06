# Add gtest fixtures as libraries

add_library(FixtureMatchingGraphs
  FixtureMatchingGraphs.cpp
  FixtureCloseGraphs.cpp
  )
target_include_directories(FixtureMatchingGraphs PUBLIC .)
target_link_libraries(FixtureMatchingGraphs
  SGCore
  ${GTEST_LIBRARIES}
  )

add_library(FixtureSquareCrossGraph
  FixtureSquareCrossGraph.cpp
  )
target_include_directories(FixtureSquareCrossGraph PUBLIC .)
target_link_libraries(FixtureSquareCrossGraph
  SGCore
  ${GTEST_LIBRARIES}
  )
