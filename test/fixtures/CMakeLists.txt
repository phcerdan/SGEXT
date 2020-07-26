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

# Create a header with a string to the folder in the source tree containing fixure images.
set(_SGEXT_FIXTURE_IMAGES_FOLDER ${PROJECT_SOURCE_DIR}/images)
set(_binary_sgext_fixture_images ${PROJECT_BINARY_DIR}/test/fixtures)
configure_file(
  sgext_fixture_images.hpp.in
  ${_binary_sgext_fixture_images}/sgext_fixture_images.hpp
  @ONLY)
# Link it to a library
add_library(FixtureImagesFolder INTERFACE)
target_include_directories(FixtureImagesFolder INTERFACE ${_binary_sgext_fixture_images})
