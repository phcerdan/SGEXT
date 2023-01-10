#!/usr/bin/env bash

# Assumes that python points to the python you want to use
# This should be managed from other script, or from CI/CD pipelines

# Be aware of dirname not working in azure pipeline bash v3,
# use inline option instead of filepath in .azure-pipelines
# https://github.com/Microsoft/azure-pipelines-tasks/issues/7934
# script_dir=$(cd $(dirname $0) || exit 1; pwd)
script_dir=$(pwd)/deploy

# Requires DEPENDENCIES_BUILD_DIR
echo "In deploy script: DEPENDENCIES_BUILD_DIR $DEPENDENCIES_BUILD_DIR"
echo "pwd: $(pwd)"
echo "script_dir: $script_dir"
echo "$(python --version)"

python -m pip install --upgrade pip
python -m pip install cmake
python -m pip install ninja
python -m pip install -r ${script_dir}/requirements-deploy.txt

  # -DCMAKE_C_FLAGS:STRING=/MP \
  # -DCMAKE_CXX_FLAGS:STRING=/MP \
pushd ${script_dir}
python setup.py bdist_wheel --build-type Release -G Ninja -- \
  -DSG_BUILD_TESTING:BOOL=OFF \
  -DSG_MODULE_ANALYZE:BOOL=ON \
  -DSG_MODULE_COMPARE:BOOL=ON \
  -DSG_MODULE_LOCATE:BOOL=ON \
  -DSG_MODULE_GENERATE:BOOL=ON \
  -DSG_MODULE_DYNAMICS:BOOL=ON \
  -DSG_MODULE_VISUALIZE:BOOL=OFF \
  -DSG_MODULE_VISUALIZE_WITH_QT:BOOL=OFF \
  -DSG_WRAP_PYTHON:BOOL=ON \
  -DDEPENDENCIES_BUILD_DIR:PATH=$DEPENDENCIES_BUILD_DIR \
  || exit 1
  # ${PYBIN}/python setup.py clean
popd
