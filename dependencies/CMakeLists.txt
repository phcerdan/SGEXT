cmake_minimum_required(VERSION 3.8.1)
project(sgext-dependencies)

# Download/build and optionally install 3rd party dependencies of SGEXT
include(ExternalProject)

set(OUTPUT_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "Base folder where builds and source folder will be installed: i.e. OUTPUT_BUILD_DIR/boost-build")

if(NOT CMAKE_BUILD_TYPE)
  set(SGEXT_CMAKE_BUILD_TYPE "Release")
else()
  set(SGEXT_CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
endif()

option(WITH_VTK "Build VTK, ITK will use it too, if WITH_ITK is enabled" ON)
option(WITH_ITK "Build ITK, DGtal will use it too." ON)
option(WITH_TBB "Build TBB. Only for Linux (gcc). Needed for gcc for include <execution> (stl_parallel)." ON)
option(CLEAN_BUILD_ARTIFACTS "Create target clean_artifacts to remove artifacts and external_project folders. Needs to call the target: make/ninja clean_artifacts." ON)
set(NUM_CORES 8 CACHE STRING "Number of cores passed to make: -jNUM_CORES")
message(STATUS "OUTPUT_BUILD_DIR: ${OUTPUT_BUILD_DIR}")
message(STATUS "NUM_CORES: ${NUM_CORES}")
message(STATUS "WITH_VTK: ${WITH_VTK}")
message(STATUS "WITH_ITK: ${WITH_ITK}")
message(STATUS "WITH_TBB: ${WITH_TBB}")
set(EP_CMAKE_C_FLAGS "" CACHE STRING "Extra CMAKE_C_FLAGS passed to all CMake based ExternalProjects")
set(EP_CMAKE_CXX_FLAGS "" CACHE STRING "Extra CMAKE_CXX_FLAGS passed to all CMake based ExternalProjects")
mark_as_advanced(EP_CMAKE_C_FLAGS)
mark_as_advanced(EP_CMAKE_CXX_FLAGS)


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
ExternalProject_Add(ep_vtk
  URL ${VTK_URL}
  URL_MD5 ${VTK_URL_MD5}
  CMAKE_GENERATOR ${CMAKE_GENERATOR}
  SOURCE_DIR ${VTK_SRC_DIR}
  BINARY_DIR ${VTK_BUILD_DIR}
  CMAKE_ARGS
  -DCMAKE_BUILD_TYPE:STRING=${SGEXT_CMAKE_BUILD_TYPE}
  -DBUILD_SHARED_LIBS:BOOL=ON
  -DCMAKE_C_FLAGS=${EP_CMAKE_C_FLAGS}
  -DCMAKE_CXX_FLAGS=${EP_CMAKE_CXX_FLAGS}
  -DBUILD_TESTING:BOOL=OFF
  -DBUILD_EXAMPLES:BOOL=OFF
  INSTALL_COMMAND ""
  )
endif()

#################################################
################### ITK #######################
#################################################
if(WITH_ITK)
  # set(ITK_GIT_TAG v5.1rc03)
  set(ITK_GIT_TAG HDopen-mingw)
  message(STATUS "ITK_VERSION: ${ITK_GIT_TAG}")
  set(ITK_BUILD_DIR ${OUTPUT_BUILD_DIR}/ITK-build)
  set(ITK_SRC_FOLDER_NAME ITK-src)
  set(ITK_SRC_DIR ${OUTPUT_BUILD_DIR}/${ITK_SRC_FOLDER_NAME})
  # set(ITK_GIT_REPOSITORY "https://github.com/InsightSoftwareConsortium/ITK.git")
  set(ITK_GIT_REPOSITORY "https://github.com/thewtex/ITK.git")
  set(_ITK_depends)
  set(_ITK_optional_cmake_args)
  if(WITH_VTK)
    set(_ITK_depends ep_vtk)
    list(APPEND _ITK_optional_cmake_args
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
    -DCMAKE_BUILD_TYPE:STRING=${SGEXT_CMAKE_BUILD_TYPE}
    -DBUILD_TESTING:BOOL=OFF
    -DCMAKE_C_FLAGS=${EP_CMAKE_C_FLAGS}
    -DCMAKE_CXX_FLAGS=${EP_CMAKE_CXX_FLAGS}
    ${_ITK_optional_cmake_args}
    INSTALL_COMMAND ""
    DEPENDS ${_ITK_depends}
    )
endif()

#################################################
################### BOOST #######################
#################################################
set(BOOST_VERSION_MAJOR 1)
set(BOOST_VERSION_MINOR 68)
set(BOOST_VERSION_PATCH 0)
set(BOOST_VERSION "${BOOST_VERSION_MAJOR}_${BOOST_VERSION_MINOR}_${BOOST_VERSION_PATCH}")
set(BOOST_VERSION_DOTS "${BOOST_VERSION_MAJOR}.${BOOST_VERSION_MINOR}.${BOOST_VERSION_PATCH}")
message(STATUS "BOOST_VERSION: ${BOOST_VERSION_DOTS}")
set(BOOST_URL "https://dl.bintray.com/boostorg/release/${BOOST_VERSION_DOTS}/source/boost_${BOOST_VERSION}.tar.gz")
set(BOOST_URL_MD5 5d8b4503582fffa9eefdb9045359c239)

if( UNIX )
  set(_boost_bootstrap_command ./bootstrap.sh)
  set(_boost_b2_command ./b2)
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
      toolset=clang
      cxxflags=-std=c++${CMAKE_CXX_STANDARD}
      cxxflags=-stdlib=libc++
      linkflags=-stdlib=libc++
      )
  endif()
endif()

set(BOOST_BUILD_DIR ${OUTPUT_BUILD_DIR}/boost-build)
ExternalProject_Add(ep_boost
  URL ${BOOST_URL}
  URL_MD5 ${BOOST_URL_MD5}
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ${_boost_bootstrap_command} --prefix=${BOOST_BUILD_DIR}
  BUILD_COMMAND  ${_boost_b2_command} install
     --without-python
     --without-log
     --threading=multi
     --link=shared
     variant=release
     ${_boost_extra_args}
     -j${NUM_CORES}
  INSTALL_COMMAND "" # Already installed in BOOST_BUILD_DIR
)

##################################################
#################### DGtal #######################
##################################################

set(DGtal_GIT_TAG 71140534477a3dd41ff8311b9176c76a52ec0b97)
message(STATUS "DGtal_VERSION: ${DGtal_GIT_TAG}")
set(DGtal_BUILD_DIR ${OUTPUT_BUILD_DIR}/DGtal-build)
set(DGtal_SRC_FOLDER_NAME DGtal-src)
set(DGtal_SRC_DIR ${OUTPUT_BUILD_DIR}/${DGtal_SRC_FOLDER_NAME})
set(DGtal_GIT_REPOSITORY "https://github.com/DGtal-team/DGtal")
set(_DGtal_depends ep_boost)
set(_DGtal_optional_cmake_args)
if(WITH_ITK)
  list(APPEND _DGtal_depends ep_itk)
  list(APPEND _DGtal_optional_cmake_args
    -DWITH_ITK:BOOL=ON
    -DITK_DIR:STRING=${ITK_BUILD_DIR}
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
      -DCMAKE_BUILD_TYPE:STRING=${SGEXT_CMAKE_BUILD_TYPE}
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DBUILD_TESTING:BOOL=OFF
      -DBUILD_EXAMPLES:BOOL=OFF
      -DBOOST_ROOT=${BOOST_BUILD_DIR}
      -DBoost_NO_BOOST_CMAKE:BOOL=ON
      # -DBOOST_DIR=${BOOST_BUILD_DIR}/lib/cmake/
      -DCMAKE_C_FLAGS=${EP_CMAKE_C_FLAGS}
      -DCMAKE_CXX_FLAGS=${EP_CMAKE_CXX_FLAGS}
      ${_DGtal_optional_cmake_args}
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
