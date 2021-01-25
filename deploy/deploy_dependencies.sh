#!/usr/bin/env bash

script_dir=$(cd $(dirname $0) || exit 1; pwd)
dependencies_src_dir=$script_dir/../dependencies
echo dependencies_src_dir: $dependencies_src_dir

# REQUIRES existing DEPENDENCIES_BUILD_DIR
# Requires CMake and Ninja
python -m pip install --upgrade pip
python -m pip install cmake
python -m pip install ninja
if [[ $OSTYPE == linux* ]]; then
  export PATH="$PATH:/home/vsts/.local/bin"
fi
echo ninja version: $(ninja --version)

if [ ! -d "$DEPENDENCIES_BUILD_DIR" ]; then
  echo ERROR: DEPENDENCIES_BUILD_DIR does not exist: $DEPENDENCIES_BUILD_DIR
  exit 1
fi
echo DEPENDENCIES_BUILD_DIR: $DEPENDENCIES_BUILD_DIR
echo OSTYPE: $OSTYPE

export CMAKE_EXECUTABLE=cmake
export SGEXT_BUILD_TYPE=Release
export CMAKE_GENERATOR=Ninja

# macos:
if [[ $OSTYPE == darwin* ]]; then
  # To find ninja and modern cmake from pip
  export PATH="/Library/Frameworks/Python.framework/Versions/2.7/bin:$PATH"
  export CMAKE_OS_VARIABLES="\
  -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.9 \
  -DCMAKE_OSX_ARCHITECTURES:STRING=x86_64 \
  -DCMAKE_CXX_STANDARD=14 \
  "
fi

# Windows: git bash (mysys) or WSL (cygwin)
# In azure-pipelines or github actions, it previously needs:
# Better usee powershell directly and use:
# call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
# to find cl.exe
if [[ $OSTYPE == msys* || $OSTYPE == cygwin ]]; then
  # export CMAKE_EXECUTABLE=cmake.exe
  # export CMAKE_GENERATOR="Visual Studio 16 2019"
  export CMAKE_OS_VARIABLES='\
  -DCMAKE_CXX_STANDARD=14 \
  -DCMAKE_C_COMPILER:PATH=cl.exe \
  -DCMAKE_CXX_COMPILER:PATH=cl.exe \
  '
fi

pushd ${DEPENDENCIES_BUILD_DIR}
$CMAKE_EXECUTABLE \
  $dependencies_src_dir \
  -DCMAKE_BUILD_TYPE=$SGEXT_BUILD_TYPE \
  -G "$CMAKE_GENERATOR" \
  $CMAKE_OS_VARIABLES \
  -DWITH_TBB:BOOL=OFF \
  -DWITH_VTK:BOOL=ON \
  -DWITH_ITK:BOOL=ON \
  || exit 1
$CMAKE_EXECUTABLE --build . \
  || exit 1
$CMAKE_EXECUTABLE --build . --target clean_artifacts \
  || exit 1
popd
