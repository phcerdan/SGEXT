# Add gtest fixtures as libraries

add_library(FixtureMatchingGraphs
  FixtureMatchingGraphs.cpp
  FixtureCloseGraphs.cpp
  )
target_include_directories(FixtureMatchingGraphs PUBLIC .)
target_link_libraries(FixtureMatchingGraphs
  SGCore
  ${Boost_LIBRARIES}
  ${GTEST_LIBRARIES}
  )
