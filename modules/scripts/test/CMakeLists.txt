set(SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
  ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  ${GTEST_LIBRARIES})
set(SG_MODULE_${SG_MODULE_NAME}_TESTS
  )
if(SG_REQUIRES_ITK)
  list(APPEND SG_MODULE_${SG_MODULE_NAME}_TESTS
    test_read_a_fixture_image.cpp
    test_reconstruct_from_distance_map.cpp
    )
endif()
# Fixture defined in test/fixtures
list(APPEND SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS FixtureImagesFolder)

SG_add_gtests()
