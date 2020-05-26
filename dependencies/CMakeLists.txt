cmake_minimum_required(VERSION 3.13.2)
project(sgext-dependencies)

# Download/build and optionally install 3rd party dependencies of SGEXT
include(ExternalProject)

set(OUTPUT_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH
  "Base folder where builds and source folder will be installed: i.e. OUTPUT_BUILD_DIR/boost-build")

if(NOT CMAKE_BUILD_TYPE)
  set(SGEXT_CMAKE_BUILD_TYPE "Release")
else()
  set(SGEXT_CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
endif()

option(WITH_VTK "Build VTK, ITK will use it too, if WITH_ITK is enabled" ON)
option(WITH_ITK "Build ITK, DGtal will use it too." ON)
option(WITH_OPENMP "Build DGtal with OPENMP support." OFF)
option(WITH_TBB "Build TBB. Only for Linux (gcc). Needed for gcc for include <execution> (stl_parallel)." ON)
option(WITH_QT5 "Enable Qt5 module for VTK and DGtal. Qt5 and QGLViewer have to be found in the system." OFF)
option(CLEAN_BUILD_ARTIFACTS "Create target clean_artifacts to remove artifacts and external_project folders. Needs to call the target: make/ninja clean_artifacts." ON)
set(NUM_CORES 8 CACHE STRING "Number of cores passed to make: -jNUM_CORES")
option(ZLIB_FORCE_STATIC "Remove the dynamic libraries after zlib install" ON)
mark_as_advanced(ZLIB_FORCE_STATIC)
message(STATUS "OUTPUT_BUILD_DIR: ${OUTPUT_BUILD_DIR}")
message(STATUS "NUM_CORES: ${NUM_CORES}")
message(STATUS "WITH_VTK: ${WITH_VTK}")
message(STATUS "WITH_ITK: ${WITH_ITK}")
message(STATUS "WITH_OPENMP: ${WITH_OPENMP}")
message(STATUS "WITH_TBB: ${WITH_TBB}")
message(STATUS "WITH_QT5: ${WITH_QT5}")


if(APPLE)
  set(_cmake_os_flags
    -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}
    -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
    )
endif()


##################################################
##################### VTK #######################
##################################################
if(WITH_VTK)
  # set(VTK_GIT_TAG v8.2.0)
  set(VTK_VERSION_MAJOR 8)
  set(VTK_VERSION_MINOR 2)
  set(VTK_VERSION_PATCH 0)
  set(VTK_VERSION_DOTS "${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR}.${VTK_VERSION_PATCH}")
  message(STATUS "VTK_VERSION: ${VTK_VERSION_DOTS}")
  set(VTK_URL "https://github.com/Kitware/VTK/archive/v${VTK_VERSION_DOTS}.zip")
  set(VTK_URL_MD5 79db2378c57b7ad5b378929f0c5b4212)
  set(VTK_BUILD_DIR ${OUTPUT_BUILD_DIR}/VTK-build)
  set(VTK_SRC_FOLDER_NAME VTK-src)
  set(VTK_SRC_DIR ${OUTPUT_BUILD_DIR}/${VTK_SRC_FOLDER_NAME})
  if(WITH_QT5)
    set(_extra_qt5_options
      -DModule_vtkGUISupportQt=ON
      )
  endif()
ExternalProject_Add(ep_vtk
  URL ${VTK_URL}
  URL_MD5 ${VTK_URL_MD5}
  CMAKE_GENERATOR ${CMAKE_GENERATOR}
  SOURCE_DIR ${VTK_SRC_DIR}
  BINARY_DIR ${VTK_BUILD_DIR}
  CMAKE_ARGS
  -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
  -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
  -DCMAKE_BUILD_TYPE:STRING=${SGEXT_CMAKE_BUILD_TYPE}
  -DBUILD_SHARED_LIBS:BOOL=OFF
  -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
  -DBUILD_TESTING:BOOL=OFF
  -DBUILD_EXAMPLES:BOOL=OFF
  ${_extra_qt5_options}
  ${_cmake_os_flags}
  INSTALL_COMMAND ""
  )
endif()

#################################################
################### ITK #######################
#################################################
if(WITH_ITK)
  set(ITK_GIT_TAG f4fbfed6f676c4ae8c0d77bda052a9699a39b76e) # Release 25 April 2020
  message(STATUS "ITK_VERSION: ${ITK_GIT_TAG}")
  set(ITK_BUILD_DIR ${OUTPUT_BUILD_DIR}/ITK-build)
  set(ITK_SRC_FOLDER_NAME ITK-src)
  set(ITK_SRC_DIR ${OUTPUT_BUILD_DIR}/${ITK_SRC_FOLDER_NAME})
  set(ITK_GIT_REPOSITORY "https://github.com/InsightSoftwareConsortium/ITK.git")
  if(WITH_VTK)
    set(_ITK_depends ep_vtk)
    set(_ITK_optional_cmake_args
      -DModule_ITKVtkGlue:BOOL=ON
      -DVTK_DIR:STRING=${VTK_BUILD_DIR}
    )
  endif()

  ExternalProject_Add(ep_itk
    GIT_REPOSITORY ${ITK_GIT_REPOSITORY}
    GIT_TAG ${ITK_GIT_TAG}
    # GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    SOURCE_DIR ${ITK_SRC_DIR}
    BINARY_DIR ${ITK_BUILD_DIR}
    CMAKE_ARGS
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_BUILD_TYPE:STRING=${SGEXT_CMAKE_BUILD_TYPE}
    -DBUILD_SHARED_LIBS:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
    ${_ITK_optional_cmake_args}
    ${_cmake_os_flags}
    INSTALL_COMMAND ""
    DEPENDS ${_ITK_depends}
    )
endif()

#################################################
################### BOOST #######################
#################################################
set(BOOST_VERSION_MAJOR 1)
set(BOOST_VERSION_MINOR 72)
set(BOOST_VERSION_PATCH 0)
set(BOOST_VERSION "${BOOST_VERSION_MAJOR}_${BOOST_VERSION_MINOR}_${BOOST_VERSION_PATCH}")
set(BOOST_VERSION_DOTS "${BOOST_VERSION_MAJOR}.${BOOST_VERSION_MINOR}.${BOOST_VERSION_PATCH}")
message(STATUS "BOOST_VERSION: ${BOOST_VERSION_DOTS}")
set(BOOST_URL "https://dl.bintray.com/boostorg/release/${BOOST_VERSION_DOTS}/source/boost_${BOOST_VERSION}.tar.gz")
set(BOOST_URL_HASH SHA256=c66e88d5786f2ca4dbebb14e06b566fb642a1a6947ad8cc9091f9f445134143f)

if( UNIX )
  set(_boost_bootstrap_command ./bootstrap.sh)
  set(_boost_b2_command ./b2)
  set(_boost_extra_args
     cxxflags=-fPIC
     )
endif()
if( WIN32 )
  set(_boost_bootstrap_command bootstrap.bat)
  set(_boost_b2_command b2.exe)
  set(_boost_extra_args
      toolset=msvc
      )
endif()
if ( APPLE )
  if(${CMAKE_CXX_STANDARD})
    set(_boost_extra_args
      cxxflags=-fPIC
      toolset=clang
      cxxflags=-std=c++${CMAKE_CXX_STANDARD}
      cxxflags=-stdlib=libc++
      cxxflags=-mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}
      linkflags=-stdlib=libc++
      )
  endif()
endif()

set(BOOST_BUILD_DIR ${OUTPUT_BUILD_DIR}/boost-build)
ExternalProject_Add(ep_boost
  URL ${BOOST_URL}
  URL_HASH ${BOOST_URL_MD5}
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ${_boost_bootstrap_command} --prefix=${BOOST_BUILD_DIR}
  BUILD_COMMAND  ${_boost_b2_command} install --prefix=${BOOST_BUILD_DIR}
     --without-python
     --without-log
     --without-coroutine
     --without-type_erasure
     --without-stacktrace
     --threading=multi
     link=static
     variant=release
     ${_boost_extra_args}
     -j${NUM_CORES}
  INSTALL_COMMAND "" # Already installed in BOOST_BUILD_DIR
)

##################################################
############## zlib (DGtal-dep) ##################
##################################################

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(_compiler_is_msvc ON)
endif()

if(_compiler_is_msvc)
  set(ZLIB_GIT_TAG cacf7f1d4e3d44d871b605da3b647f07d718623f) # Version 1.2.11
  message(STATUS "ZLIB_VERSION: ${ZLIB_GIT_TAG} : Version 1.2.11")
  set(ZLIB_BUILD_DIR ${OUTPUT_BUILD_DIR}/zlib-build)
  set(ZLIB_INSTALL_DIR ${OUTPUT_BUILD_DIR}/zlib)
  set(ZLIB_SRC_FOLDER_NAME zlib-src)
  set(ZLIB_SRC_DIR ${OUTPUT_BUILD_DIR}/${ZLIB_SRC_FOLDER_NAME})
  set(ZLIB_GIT_REPOSITORY "https://github.com/madler/zlib")

  ExternalProject_Add(ep_zlib
    GIT_REPOSITORY ${ZLIB_GIT_REPOSITORY}
    GIT_TAG ${ZLIB_GIT_TAG}
    # GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    SOURCE_DIR ${ZLIB_SRC_DIR}
    BINARY_DIR ${ZLIB_BUILD_DIR}
    CMAKE_ARGS
        -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
        -DCMAKE_BUILD_TYPE:STRING=${SGEXT_CMAKE_BUILD_TYPE}
        -DBUILD_SHARED_LIBS:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX=${ZLIB_INSTALL_DIR}
        ${_cmake_os_flags}
    )
  if(ZLIB_FORCE_STATIC)
    ExternalProject_Add_Step(
      ep_zlib zlib_remove_dll
      COMMENT "Remove zlib.lib and zlib.dll, leaves only zlibstatic.lib"
      DEPENDEES install
      COMMAND ${CMAKE_COMMAND} -E remove -f ${ZLIB_INSTALL_DIR}/lib/zlib.lib ${ZLIB_INSTALL_DIR}/bin/zlib.dll
      )
  endif()

endif()

##################################################
#################### DGtal #######################
##################################################

# set(DGtal_GIT_TAG 92ab4d0d62f0e5490f1890efdf904547d17515b9) # 25-May-2020
set(DGtal_GIT_TAG add_dgtal_lut) # 26-May-2020
message(STATUS "DGtal_VERSION: ${DGtal_GIT_TAG}")
set(DGtal_BUILD_DIR ${OUTPUT_BUILD_DIR}/DGtal-build)
set(DGtal_SRC_FOLDER_NAME DGtal-src)
set(DGtal_SRC_DIR ${OUTPUT_BUILD_DIR}/${DGtal_SRC_FOLDER_NAME})
set(DGtal_GIT_REPOSITORY "https://github.com/phcerdan/DGtal")
set(_DGtal_depends ep_boost)
set(_DGtal_optional_cmake_args)
if(WITH_ITK)
  list(APPEND _DGtal_depends ep_itk)
  list(APPEND _DGtal_optional_cmake_args
    -DWITH_ITK:BOOL=ON
    -DITK_DIR:STRING=${ITK_BUILD_DIR}
    )
endif()
if(WITH_OPENMP)
  list(APPEND _DGtal_optional_cmake_args
    -DWITH_OPENMP:BOOL=ON
    )
endif()
if(_compiler_is_msvc)
  list(APPEND _DGtal_depends ep_zlib)
  list(APPEND _DGtal_optional_cmake_args
    -DZLIB_ROOT=${ZLIB_INSTALL_DIR}
    )
endif()
if(WITH_QT5)
  list(APPEND _DGtal_optional_cmake_args
    -DWITH_QT5:BOOL=ON
    -DWITH_QGLVIEWER:BOOL=ON
    )
endif()

ExternalProject_Add(ep_dgtal
  GIT_REPOSITORY ${DGtal_GIT_REPOSITORY}
  GIT_TAG ${DGtal_GIT_TAG}
  # GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
  CMAKE_GENERATOR ${CMAKE_GENERATOR}
  SOURCE_DIR ${DGtal_SRC_DIR}
  BINARY_DIR ${DGtal_BUILD_DIR}
  CMAKE_ARGS
      -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
      -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
      -DCMAKE_BUILD_TYPE:STRING=${SGEXT_CMAKE_BUILD_TYPE}
      -DBUILD_SHARED_LIBS:BOOL=OFF
      -DBUILD_TESTING:BOOL=OFF
      -DBUILD_EXAMPLES:BOOL=OFF
      -DWITH_DGtalLUT:BOOL=ON
      -DCMAKE_FIND_PACKAGE_PREFER_CONFIG:BOOL=ON
      -DBoost_DIR=${BOOST_BUILD_DIR}/lib/cmake/Boost-${BOOST_VERSION_DOTS}
      ${_DGtal_optional_cmake_args}
      ${_cmake_os_flags}
  INSTALL_COMMAND ""
  DEPENDS ${_DGtal_depends}
  )

##################################################
#################### TBB #######################
##################################################
if(WITH_TBB)
  if(UNIX AND NOT APPLE)
    set(TBB_VERSION "2020.2")
    message(STATUS "TBB_VERSION: ${TBB_VERSION}")
    set(TBB_URL "https://github.com/intel/tbb/archive/v${TBB_VERSION}.tar.gz")
    set(TBB_URL_MD5 5af6f6c2a24c2043e62e47205e273b1f)
    set(TBB_BUILD_DIR ${OUTPUT_BUILD_DIR}/tbb-build)
    set(TBB_SRC_FOLDER_NAME tbb-src)
    set(TBB_SRC_DIR ${OUTPUT_BUILD_DIR}/${TBB_SRC_FOLDER_NAME})
    set(TBB_MAKE_PROGRAM make) # In windows, if needed, might be changed to gmake
    ExternalProject_Add(ep_tbb
      URL ${TBB_URL}
      URL_MD5 ${TBB_URL_MD5}
      SOURCE_DIR ${TBB_SRC_DIR}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ""
      BUILD_COMMAND
      ${TBB_MAKE_PROGRAM} -j${NUM_CORES}
      INSTALL_COMMAND ""
      )
    # This is the pseudo-install command but post-build instead
    add_custom_command(TARGET ep_tbb
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND}
      -DTBB_SRC_DIR=${TBB_SRC_DIR}
      -DTBB_INSTALL_DIR=${TBB_BUILD_DIR}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/install_tbb_from_build_folder.cmake
      )
  else()
    message(WARNING "Not installing TBB for your platform (only needed in linux for gcc)")
  endif()
endif()

##################################################
#################### CLEAN #######################
##################################################
if(CLEAN_BUILD_ARTIFACTS)
  # set(_clean_artifacts_depends ep_dgtal ep_boost)
  # if(WITH_ITK)
  #   list(APPEND _clean_artifacts_depends ep_itk)
  # endif()
  # if(WITH_VTK)
  #   list(APPEND _clean_artifacts_depends ep_vtk)
  # endif()
  # if(WITH_TBB)
  #   list(APPEND _clean_artifacts_depends ep_tbb)
  # endif()
  #   # DEPENDS ${_clean_artifacts_depends}

  # Dummy target
  add_custom_target(clean_artifacts
    )
  add_custom_command(TARGET clean_artifacts
    POST_BUILD
    COMMAND
    ${CMAKE_COMMAND}
    -DOUTPUT_BUILD_DIR=${OUTPUT_BUILD_DIR}
    -DITK_SRC_DIR=${ITK_SRC_DIR}
    -DDGtal_SRC_DIR=${DGtal_SRC_DIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/clean_build_artifacts.cmake
    )
endif()
