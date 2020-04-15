# Require ${DEPENDENCIES_BUILD_DIR} set to the OUTPUT_BUILD_DIR where dependencies where built.
# Using sub-project ./dependencies
# mkdir build-dependencies
# cmake -DOUTPUT_BUILD_DIR=/path/to ${SGEXT_SOURCE_DIR}/dependencies
# cmake . --build
# This will create folder with the dependencies of SGEXT, such as : OUTPUT_BUILD_DIR/VTK-build/src, DGtal-build/src, boost-build, ITK-build/src, and tbb-build.

set(_dependencies_vtk_build ${DEPENDENCIES_BUILD_DIR}/VTK-build)
set(_dependencies_itk_build ${DEPENDENCIES_BUILD_DIR}/ITK-build)
set(_dependencies_boost_build ${DEPENDENCIES_BUILD_DIR}/boost-build)
set(_dependencies_dgtal_build ${DEPENDENCIES_BUILD_DIR}/DGtal-build)
set(_dependencies_tbb_build ${DEPENDENCIES_BUILD_DIR}/tbb-build)

if(EXISTS ${_dependencies_vtk_build} AND NOT VTK_DIR)
  set(VTK_DIR ${DEPENDENCIES_BUILD_DIR}/VTK-build CACHE PATH "Folder to find vtk-config.cmake")
endif()
if(EXISTS ${_dependencies_itk_build} AND NOT ITK_DIR)
  set(ITK_DIR ${DEPENDENCIES_BUILD_DIR}/ITK-build CACHE PATH "Folder to find itk-config.cmake")
endif()
if(EXISTS ${_dependencies_boost_build} AND NOT Boost_DIR)
  set(Boost_NO_BOOST_CMAKE ON CACHE BOOL "Ignore Boost_DIR (used in modern boost to use boost-config.cmake)")
  set(BOOST_ROOT ${DEPENDENCIES_BUILD_DIR}/boost-build CACHE PATH "Folder to find include and lib folders of boost. Deprecated, modern boost would use Boost_DIR.")
endif()
if(EXISTS ${_dependencies_dgtal_build} AND NOT DGtal_DIR)
  set(DGtal_DIR ${DEPENDENCIES_BUILD_DIR}/DGtal-build CACHE PATH "Folder to find DGtal-config.cmake")
endif()
if(EXISTS ${_dependencies_tbb_build} AND NOT (TBB_DIR OR TBB_ROOT))
  # set(TBB_DIR ${DEPENDENCIES_BUILD_DIR}/tbb-build/lib/cmake/TBB CACHE PATH "Folder to find tbb-config.cmake")
  set(TBB_ROOT ${DEPENDENCIES_BUILD_DIR}/tbb-build CACHE PATH "Folder to find include and lib of TBB. Deprecated, modern tbb should use tbb-config.cmake")
endif()
