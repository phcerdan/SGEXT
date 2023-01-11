#!/usr/bin/env bash

# -----------------------------------------------------------------------
# These variables are set in common script:
#
ARCH=""
PYBINARIES=""

script_dir=$(cd $(dirname $0) || exit 1; pwd)
source "${script_dir}/manylinux-build-common.sh"
# -----------------------------------------------------------------------

echo "BOOST_BUILD_DIR: ${BOOST_BUILD_DIR}"
echo "DGtal_BUILD_DIR: ${DGtal_BUILD_DIR}"
echo "ITK_BUILD_DIR: ${ITK_BUILD_DIR}"
echo "VTK_BUILD_DIR: ${VTK_BUILD_DIR}"

deploy_dir=${script_dir}/..
pushd ${deploy_dir}
# Compile wheels re-using standalone project and archive cache
for PYBIN in "${PYBINARIES[@]}"; do
    PYTHON_EXECUTABLE=${PYBIN}/python
    PYTHON_INCLUDE_DIR=$( find -L ${PYBIN}/../include/ -name Python.h -exec dirname {} \; )
    PYTHON_INCLUDE_DIRS=${PYTHON_INCLUDE_DIR}

    echo ""
    echo "PYTHON_EXECUTABLE:${PYTHON_EXECUTABLE}"
    echo "PYTHON_INCLUDE_DIR:${PYTHON_INCLUDE_DIR}"

    if [[ -e /work/deploy/requirements-deploy.txt ]]; then
      ${PYBIN}/pip install --upgrade -r /work/deploy/requirements-deploy.txt
    elif [[ -e ${deploy_dir}/requirements-deploy.txt ]]; then
      ${PYBIN}/pip install --upgrade -r ${deploy_dir}/requirements-deploy.txt
    else
        echo "requirements-deploy file not found. Need to update pip. Exiting" 1>&2
        exit 1
    fi

    # TODO: Switch BOOST_ROOT to these two when CMake is at least 3.15 and remove Boost_ROOT
    # -DCMAKE_FIND_PACKAGE_PREFER_CONFIG:BOOL=ON \
    # -DBoost_DIR:STRING=${BOOST_CMAKE_CONFIG_FOLDER} \
    ${PYBIN}/python setup.py bdist_wheel --build-type Release -G Ninja -- \
      -DCMAKE_CXX_COMPILER_TARGET:STRING=$(uname -p)-linux-gnu \
      -DSG_BUILD_TESTING:BOOL=OFF \
      -DSG_MODULE_ANALYZE:BOOL=ON \
      -DSG_MODULE_COMPARE:BOOL=ON \
      -DSG_MODULE_LOCATE:BOOL=ON \
      -DSG_MODULE_GENERATE:BOOL=ON \
      -DSG_MODULE_DYNAMICS:BOOL=ON \
      -DSG_MODULE_VISUALIZE:BOOL=ON \
      -DSG_MODULE_VISUALIZE_WITH_QT:BOOL=OFF \
      -DBOOST_ROOT=${BOOST_BUILD_DIR} \
      -DITK_DIR:STRING=${ITK_BUILD_DIR} \
      -DDGtal_DIR:STRING=${DGtal_BUILD_DIR} \
      -DSG_WRAP_PYTHON:BOOL=ON \
      -DPYTHON_EXECUTABLE:FILEPATH=${PYTHON_EXECUTABLE} \
      -DPYTHON_INCLUDE_DIR=${PYTHON_INCLUDE_DIR} \
    || exit 1
    # ${PYBIN}/python setup.py clean
done

# auditwheel will bundle shared libraries in the wheel,
# but they have to be found first using LD_LIBRARY_PATH
export DEPENDENCIES_LD_LIBRARY_PATH="/work/VTK-build/lib64:/work/VTK-build/lib:/work/boost-build/lib:/work/DGtal-build"
# This step will fixup the wheel switching from 'linux' to 'manylinux2014' tag and include third party libraries
for whl in dist/*linux_$(uname -p).whl; do
    LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${DEPENDENCIES_LD_LIBRARY_PATH} auditwheel repair ${whl} -w /work/dist/
    rm ${whl}
done
popd
