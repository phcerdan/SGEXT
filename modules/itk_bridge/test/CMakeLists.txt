set(SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
  ${SG_MODULE_${SG_MODULE_NAME}_LIBRARY}
  ${SG_MODULE_${SG_MODULE_NAME}_DEPENDS}
  ${GTEST_LIBRARIES})
set(SG_MODULE_${SG_MODULE_NAME}_TESTS
  )
if(SG_MODULE_SCRIPTS)
  list(APPEND SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS
    SGScripts
    )
  list(APPEND SG_MODULE_${SG_MODULE_NAME}_TESTS
    test_create_vertex_to_radius_map.cpp
    )
endif()
# Fixture defined in test/fixtures
list(APPEND SG_MODULE_${SG_MODULE_NAME}_TEST_DEPENDS FixtureImagesFolder)

SG_add_gtests()
