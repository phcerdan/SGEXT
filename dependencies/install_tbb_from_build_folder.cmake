# BUILD_IN_SOURCE
# TBB_SRC_DIR
# install folder
# Assuming TBB_SRC_DIR is equal to TBB_BUILD_DIR (BUILD_IN_SOURCE)
if(NOT TBB_SRC_DIR)
  message(FATAL_ERROR "Provide TBB_SRC_DIR to the script")
endif()
if(NOT TBB_INSTALL_DIR)
  message(FATAL_ERROR "Provide TBB_INSTALL_DIR to the script")
endif()

file(MAKE_DIRECTORY ${TBB_INSTALL_DIR})
file(MAKE_DIRECTORY ${TBB_INSTALL_DIR}/lib)

file(GLOB tbb_so_files ${TBB_SRC_DIR}/build/*/*.so*)
if(NOT tbb_so_files)
  message(FATAL_ERROR "tbb .so libraries not found. Was build successful?")
endif()
file(COPY ${tbb_so_files} DESTINATION ${TBB_INSTALL_DIR}/lib)
file(COPY ${TBB_SRC_DIR}/include/tbb DESTINATION ${TBB_INSTALL_DIR}/include)

execute_process(
  COMMAND
  ${CMAKE_COMMAND}
  -DINSTALL_DIR=${TBB_INSTALL_DIR}/lib/cmake/TBB
  -DSYSTEM_NAME=Linux
  -DTBB_VERSION_FILE=${TBB_INSTALL_DIR}/include/tbb/tbb_stddef.h
  -P cmake/tbb_config_installer.cmake
  WORKING_DIRECTORY ${TBB_SRC_DIR}
)
