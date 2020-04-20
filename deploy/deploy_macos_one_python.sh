#!/usr/bin/env bash

# Assumes that python points to the python you want to use
# This should be managed from other script, or from CI/CD pipelines

script_dir=$(cd $(dirname $0) || exit 1; pwd)

echo ""
echo "$(python --version)"

python -m pip install cmake
python -m pip install ninja
python -m pip install delocate
python -m pip install -r requirements-deploy.txt

pushd ${script_dir}
python setup.py bdist_wheel --build-type Release -G Ninja -- \
  -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.9 \
  -DCMAKE_OSX_ARCHITECTURES:STRING=x86_64 \
  -DCMAKE_CXX_STANDARD=14 \
  -DSG_BUILD_TESTING:BOOL=OFF \
  -DSG_MODULE_ANALYZE:BOOL=ON \
  -DSG_MODULE_COMPARE:BOOL=ON \
  -DSG_MODULE_LOCATE:BOOL=ON \
  -DSG_MODULE_GENERATE:BOOL=ON \
  -DSG_MODULE_DYNAMICS:BOOL=ON \
  -DSG_MODULE_VISUALIZE:BOOL=OFF \
  -DSG_MODULE_VISUALIZE_WITH_QT:BOOL=OFF \
  -DBOOST_ROOT=${BOOST_BUILD_DIR} \
  -DITK_DIR:STRING=${ITK_BUILD_DIR} \
  -DDGtal_DIR:STRING=${DGtal_BUILD_DIR} \
  -DSG_WRAP_PYTHON:BOOL=ON \

  || exit 1
  # ${PYBIN}/python setup.py clean

delocate-listdeps $PWD/dist/*.whl # lists library dependencies
delocate-wheel $PWD/dist/*.whl # copies library dependencies into wheel
popd
