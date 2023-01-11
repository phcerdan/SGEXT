#!/usr/bin/env bash

# Assumes that python points to the python you want to use
# This should be managed from other script, or from CI/CD pipelines

script_dir=$(cd $(dirname $0) || exit 1; pwd)

# Requires DEPENDENCIES_BUILD_DIR
echo "In deploy script: DEPENDENCIES_BUILD_DIR $DEPENDENCIES_BUILD_DIR"
echo ""
echo "$(python --version)"

python -m pip install --upgrade pip
python -m pip install cmake
python -m pip install ninja
python -m pip install delocate
python -m pip install -r $script_dir/requirements-deploy.txt


pushd ${script_dir}
python setup.py bdist_wheel --build-type Release -G Ninja -- \
  -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.9 \
  -DCMAKE_OSX_ARCHITECTURES:STRING=x86_64 \
  -DSG_BUILD_TESTING:BOOL=OFF \
  -DSG_MODULE_ANALYZE:BOOL=ON \
  -DSG_MODULE_COMPARE:BOOL=ON \
  -DSG_MODULE_LOCATE:BOOL=ON \
  -DSG_MODULE_GENERATE:BOOL=ON \
  -DSG_MODULE_DYNAMICS:BOOL=ON \
  -DSG_MODULE_VISUALIZE:BOOL=ON \
  -DSG_MODULE_VISUALIZE_WITH_QT:BOOL=OFF \
  -DSG_WRAP_PYTHON:BOOL=ON \
  -DDEPENDENCIES_BUILD_DIR:PATH=$DEPENDENCIES_BUILD_DIR \
  -DBoost_USE_STATIC_LIBS:BOOL=ON \
  || exit 1
  # ${PYBIN}/python setup.py clean

export DEPENDENCIES_LD_LIBRARY_PATH="${DEPENDENCIES_BUILD_DIR}/VTK-build/lib:${DEPENDENCIES_BUILD_DIR}/boost-build/lib:${DEPENDENCIES_BUILD_DIR}/DGtal-build"

DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${DEPENDENCIES_LD_LIBRARY_PATH} delocate-listdeps $PWD/dist/*.whl # lists library dependencies
DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${DEPENDENCIES_LD_LIBRARY_PATH} delocate-wheel $PWD/dist/*.whl # copies library dependencies into wheel
popd
