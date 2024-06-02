cmake_minimum_required(VERSION 3.13.2)
project(sgext-dependencies)

if(POLICY CMP0057)
  cmake_policy(SET CMP0057 NEW) #CMP0057 allow IN_LIST inside ifs()
endif()
# ExternalProject warning related to DOWNLOAD_EXTRACT_TIMESTAMP
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

# Download/build and optionally install 3rd party dependencies of SGEXT
include(ExternalProject)

set(OUTPUT_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH
  "Base folder where builds and source folder will be installed: i.e. OUTPUT_BUILD_DIR/boost-build")

# This will handle also the case for MultiConfig generators
# Issue to propagate config to ExternalProject_Add: https://gitlab.kitware.com/cmake/cmake/-/issues/17645
if(NOT CMAKE_BUILD_TYPE)
  set(SGEXT_CMAKE_BUILD_TYPE "Release")
else()
  set(SGEXT_CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
endif()

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (isMultiConfig)
  set(_propagate_build_type "--config ${SGEXT_CMAKE_BUILD_TYPE}")
else()
  set(_propagate_build_type "-DCMAKE_BUILD_TYPE=${SGEXT_CMAKE_BUILD_TYPE}")
endif()

option(WITH_VTK "Build VTK, ITK will use it too, if WITH_ITK is enabled" ON)
option(USE_EXTERNAL_VTK "Do not install VTK here, but assume VTK_DIR is provided for other dependencies" OFF)
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

if(USE_EXTERNAL_VTK AND (WITH_VTK))
  message(FATAL_ERROR "USE_EXTERNAL_VTK is ON, but either WITH_VTK is also ON. "
    "Either set USE_EXTERNAL_VTK to OFF or set WITH_VTK to OFF")
endif()
if(USE_EXTERNAL_VTK AND NOT VTK_DIR)
  message(FATAL_ERROR "USE_EXTERNAL_VTK=ON requires providing a VTK_DIR")
endif()
if(USE_EXTERNAL_VTK)
  set(VTK_BUILD_DIR ${VTK_DIR})
endif()

if(APPLE)
  set(_cmake_os_flags
    -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}
    -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
    )
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # Used to compile zlib in windows with msvc
  set(_compiler_is_msvc ON)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # Used for boost in unix when the compiler is clang
  set(_compiler_is_clang ON)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # Used to disable compilation of DGtal with gcc10.1
  set(_compiler_is_gnu ON)
endif()

##################################################
##################### VTK #######################
##################################################
# Install VTKv9 from master
set(VTK_GIT_TAG v9.3.0.rc1) # August 2023
message(STATUS "VTK_VERSION: ${VTK_GIT_TAG}")
set(VTK_BUILD_DIR ${OUTPUT_BUILD_DIR}/VTK-build)
set(VTK_SRC_FOLDER_NAME VTK-src)
set(VTK_SRC_DIR ${OUTPUT_BUILD_DIR}/${VTK_SRC_FOLDER_NAME})
set(VTK_GIT_REPOSITORY "https://github.com/Kitware/VTK.git")
if(WITH_QT5)
  set(_extra_qt5_options
    -DModule_vtkGUISupportQt=ON
    )
endif()
ExternalProject_Add(ep_vtk
  GIT_REPOSITORY ${VTK_GIT_REPOSITORY}
  GIT_TAG ${VTK_GIT_TAG}
  GIT_PROGRESS TRUE
  CMAKE_GENERATOR ${CMAKE_GENERATOR}
  SOURCE_DIR ${VTK_SRC_DIR}
  BINARY_DIR ${VTK_BUILD_DIR}
  CMAKE_ARGS
  -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
  -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
  -DBUILD_SHARED_LIBS:BOOL=OFF
  -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
  -DVTK_BUILD_TESTING:BOOL=OFF
  -DVTK_MODULE_ENABLE_VTK_RenderingContextOpenGL2:STRING=WANT
  ${_propagate_build_type}
  ${_extra_qt5_options}
  ${_cmake_os_flags}
  INSTALL_COMMAND ""
  )

#################################################
################### ITK #######################
#################################################
if(WITH_ITK)
  set(ITK_GIT_TAG v5.4rc01) # 28 Jul 2023
  message(STATUS "ITK_VERSION: ${ITK_GIT_TAG}")
  set(ITK_BUILD_DIR ${OUTPUT_BUILD_DIR}/ITK-build)
  set(ITK_SRC_FOLDER_NAME ITK-src)
  set(ITK_SRC_DIR ${OUTPUT_BUILD_DIR}/${ITK_SRC_FOLDER_NAME})
  set(ITK_GIT_REPOSITORY "https://github.com/InsightSoftwareConsortium/ITK.git")
  if(WITH_VTK OR USE_EXTERNAL_VTK)
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
    -DBUILD_SHARED_LIBS:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
    ${_propagate_build_type}
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
set(BOOST_VERSION_MINOR 85)
set(BOOST_VERSION_PATCH 0)
set(BOOST_VERSION "${BOOST_VERSION_MAJOR}_${BOOST_VERSION_MINOR}_${BOOST_VERSION_PATCH}")
set(BOOST_VERSION_DOTS "${BOOST_VERSION_MAJOR}.${BOOST_VERSION_MINOR}.${BOOST_VERSION_PATCH}")
message(STATUS "BOOST_VERSION: ${BOOST_VERSION_DOTS}")
set(BOOST_GIT_REPOSITORY "https://github.com/boostorg/boost")
set(BOOST_GIT_TAG "boost-${BOOST_VERSION_DOTS}")
set(BOOST_URL "https://boostorg.jfrog.io/artifactory/main/release/${BOOST_VERSION_DOTS}/source/boost_${BOOST_VERSION}.tar.gz")
set(BOOST_URL_HASH SHA256=be0d91732d5b0cc6fbb275c7939974457e79b54d6f07ce2e3dfdd68bef883b0b)

# SGEXT_CMAKE_BUILD_TYPE always exists (even in MultiConfig)
string(TOLOWER ${SGEXT_CMAKE_BUILD_TYPE} _boost_variant)
if(${_boost_variant} STREQUAL "relwithdebinfo")
  set(_boost_variant "release")
endif()

if( UNIX )
  set(_boost_bootstrap_command ./bootstrap.sh)
  set(_boost_b2_command ./b2)
  set(_boost_extra_args
     cxxflags=-fPIC
     )
  if(_compiler_is_clang)
    set(_boost_extra_args
      cxxflags=-fPIC
      toolset=clang
      )
  endif()
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
      # Solves: ld: warning: direct access in function ... from file ... to global weak symbol
      # Needed because we are building with link=static
      cxxflags=-fvisibility=hidden
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
     variant=${_boost_variant}
     ${_boost_extra_args}
     -j${NUM_CORES}
  INSTALL_COMMAND "" # Already installed in BOOST_BUILD_DIR
)

##################################################
############## zlib (DGtal-dep) ##################
##################################################

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
        -DBUILD_SHARED_LIBS:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX=${ZLIB_INSTALL_DIR}
        ${_propagate_build_type}
        ${_cmake_os_flags}
    )
  if(ZLIB_FORCE_STATIC)
    ExternalProject_Add_Step(
      ep_zlib zlib_remove_dll
      COMMENT "Remove zlib.lib and zlib.dll, leaves only zlibstatic.lib"
      DEPENDEES install
      COMMAND ${CMAKE_COMMAND} -E remove -f ${ZLIB_INSTALL_DIR}/lib/zlib.lib ${ZLIB_INSTALL_DIR}/bin/zlib.dll ${ZLIB_INSTALL_DIR}/lib/zlibd.lib ${ZLIB_INSTALL_DIR}/bin/zlibd.dll
      )
  endif()

endif()

##################################################
#################### DGtal #######################
##################################################
if(_compiler_is_gnu AND CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 10.1)
  message("CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
  message(FATAL_ERROR "DGtal cannot be compiled in GCC 10.1 due to a bug in the compiler."
    "Try llvm, or other version of gcc. Issue: https://github.com/DGtal-team/DGtal/issues/1501")
endif()


set(DGtal_GIT_TAG 9f5f065b3ace5566e1d7257ed263767e70474a1c) # DGtal 1.2 with patches
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
      -DBUILD_SHARED_LIBS:BOOL=OFF
      -DBUILD_TESTING:BOOL=OFF
      -DBUILD_EXAMPLES:BOOL=OFF
      -DCMAKE_FIND_PACKAGE_PREFER_CONFIG:BOOL=ON
      -DBoost_DIR=${BOOST_BUILD_DIR}/lib/cmake/Boost-${BOOST_VERSION_DOTS}
      -DDGTAL_CONFIG_HINTS:BOOL=OFF
      ${_propagate_build_type}
      ${_DGtal_optional_cmake_args}
      ${_cmake_os_flags}
  INSTALL_COMMAND ""
  DEPENDS ${_DGtal_depends}
  )

##################################################
#################### TBB #######################
##################################################
if(WITH_TBB)
  if(UNIX AND NOT APPLE AND NOT _compiler_is_clang)
    set(TBB_VERSION "2021.10.0")
    message(STATUS "TBB_VERSION: ${TBB_VERSION}")
    set(TBB_URL "https://github.com/oneapi-src/oneTBB/archive/v${TBB_VERSION}/onetbb-${TBB_VERSION}.tar.gz")
    set(TBB_URL_MD5 2caf55b3d35b53edccb33ecaf0f23402)
    set(TBB_BUILD_DIR ${OUTPUT_BUILD_DIR}/onetbb-build)
    set(TBB_SRC_FOLDER_NAME onetbb-src)
    set(TBB_SRC_DIR ${OUTPUT_BUILD_DIR}/${TBB_SRC_FOLDER_NAME})
    ExternalProject_Add(ep_onetbb
      URL ${TBB_URL}
      SOURCE_DIR ${TBB_SRC_DIR}
      BINARY_DIR ${TBB_BUILD_DIR}
      URL_HASH MD5=${TBB_URL_MD5}
      CMAKE_ARGS
        -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
        -DBUILD_SHARED_LIBS:BOOL=OFF
        -DTBB_STRICT:BOOL=OFF
        ${_propagate_build_type}
        ${_cmake_os_flags}
      INSTALL_COMMAND ""
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
