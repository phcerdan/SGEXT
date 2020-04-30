# Useful to trim down the size of the dependencies build folder
# Used in docker images.
if(NOT OUTPUT_BUILD_DIR)
  message(FATAL_ERROR "Provide -DOUTPUT_BUILD_DIR to this script pointing to the BINARY_DIR")
endif()
# VTK
file(REMOVE_RECURSE ${OUTPUT_BUILD_DIR}/ep_vtk-prefix)
# ITK
file(REMOVE_RECURSE ${OUTPUT_BUILD_DIR}/ep_itk-prefix)
file(REMOVE_RECURSE ${ITK_SRC_DIR}/.git)
# Boost
file(REMOVE_RECURSE ${OUTPUT_BUILD_DIR}/ep_boost-prefix)
# DGtal
file(REMOVE_RECURSE ${OUTPUT_BUILD_DIR}/ep_dgtal-prefix)
file(REMOVE_RECURSE ${DGtal_SRC_DIR}/.git)
# TBB
file(REMOVE_RECURSE ${OUTPUT_BUILD_DIR}/ep_tbb-prefix)
file(REMOVE_RECURSE ${OUTPUT_BUILD_DIR}/tbb-src)

file(GLOB compilation_objects ${OUTPUT_BUILD_DIR}/*.o ${OUTPUT_BUILD_DIR}/*.obj)
if(compilation_objects)
  file(REMOVE ${compilation_objects})
  message(STATUS "compilation_objects removed")
endif()

