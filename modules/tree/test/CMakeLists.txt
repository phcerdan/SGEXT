set(SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
  ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  ${GTEST_LIBRARIES})

set(SG_MODULE_${SG_MODULE_NAME}_TESTS
  test_tree_generation.cpp
  )

# Fixtures folder
list(APPEND SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS FixtureImagesFolder)

SG_add_gtests()
