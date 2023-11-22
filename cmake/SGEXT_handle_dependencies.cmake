# Require ${DEPENDENCIES_BUILD_DIR} set to the OUTPUT_BUILD_DIR where dependencies where built.
# Using sub-project ./dependencies
# mkdir build-dependencies
# cmake -DOUTPUT_BUILD_DIR=/path/to ${SGEXT_SOURCE_DIR}/dependencies
# cmake . --build
# This will create folder with the dependencies of SGEXT, such as : OUTPUT_BUILD_DIR/VTK-build/src, DGtal-build/src, boost-build, ITK-build/src, and tbb-build.

message(STATUS "Setting dependencies if DEPENDENCIES_BUILD_DIR is set")
message(STATUS "DEPENDENCIES_BUILD_DIR: ${DEPENDENCIES_BUILD_DIR}")

set(_dependencies_vtk_build ${DEPENDENCIES_BUILD_DIR}/VTK-build)
set(_dependencies_itk_build ${DEPENDENCIES_BUILD_DIR}/ITK-build)
set(_dependencies_boost_build ${DEPENDENCIES_BUILD_DIR}/boost-build)
set(_dependencies_dgtal_build ${DEPENDENCIES_BUILD_DIR}/DGtal-build)
set(_dependencies_tbb_build ${DEPENDENCIES_BUILD_DIR}/tbb-build)
# DGtal needs zlib in Windows
set(_dependencies_zlib ${DEPENDENCIES_BUILD_DIR}/zlib)

if(EXISTS ${_dependencies_vtk_build} AND NOT VTK_DIR)
  set(VTK_ROOT ${_dependencies_vtk_build} CACHE PATH "Folder to find include and lib folders of vtk.")
  set(VTK_DIR ${_dependencies_vtk_build} CACHE PATH "Folder to find vtk-config.cmake")
endif()
if(EXISTS ${_dependencies_itk_build} AND NOT ITK_DIR)
  set(ITK_ROOT ${_dependencies_vtk_build} CACHE PATH "Folder to find include and lib folders of vtk.")
  set(ITK_DIR ${_dependencies_itk_build} CACHE PATH "Folder to find itk-config.cmake")
endif()
if(EXISTS ${_dependencies_boost_build} AND NOT Boost_DIR)
  # Get the Boost-${BOOST_VERSION} folder (version is unknown to this project)
  file(GLOB _boost_cmake_folder
    ${_dependencies_boost_build}/lib/cmake/Boost-*)
  # set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON CACHE BOOL "CMake prefers to use CONFIG files. Set by SGEXT_handle_dependencies, used for boost-config.cmake.")
  set(Boost_ROOT ${_dependencies_boost_build} CACHE PATH "Folder to find include and lib folders of boost.")
  set(Boost_DIR ${_boost_cmake_folder} CACHE PATH "Folder to find Boost-config.cmake")
  if(Boost_NO_BOOST_CMAKE)
    set(BOOST_ROOT ${_dependencies_boost_build} CACHE PATH "Folder to find include and lib folders of boost.")
  endif()
endif()
if(EXISTS ${_dependencies_dgtal_build} AND NOT DGtal_DIR)
  set(DGtal_ROOT ${_dependencies_dgtal_build} CACHE PATH "Folder to find DGtal-config.cmake")
  set(DGtal_DIR ${_dependencies_dgtal_build} CACHE PATH "Folder to find DGtal-config.cmake")
endif()
if(EXISTS ${_dependencies_tbb_build} AND NOT (TBB_DIR OR TBB_ROOT))
  # set(TBB_DIR ${DEPENDENCIES_BUILD_DIR}/tbb-build/lib/cmake/TBB CACHE PATH "Folder to find tbb-config.cmake")
  set(TBB_ROOT ${_dependencies_tbb_build} CACHE PATH "Folder to find include and lib of TBB. Deprecated, modern tbb should use tbb-config.cmake")
endif()
if(EXISTS ${_dependencies_zlib} AND NOT (ZLIB_DIR OR ZLIB_ROOT))
  set(ZLIB_ROOT ${_dependencies_zlib} CACHE PATH "Provide ZLIB_ROOT for FindZLIB.cmake. Needed by DGtal on Windows.")
endif()
