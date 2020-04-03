#!/usr/bin/env bash

# -----------------------------------------------------------------------
# These variables are set in common script:
#
ARCH=""
PYBINARIES=""
Python3_LIBRARY=""

script_dir=$(cd $(dirname $0) || exit 1; pwd)
source "${script_dir}/manylinux-build-common.sh"
# Install prerequirements
# mkdir -p /work/tools
# pushd /work/tools > /dev/null 2>&1
# curl https://data.kitware.com/api/v1/file/5c0aa4b18d777f2179dd0a71/download -o doxygen-1.8.11.linux.bin.tar.gz
# tar -xvzf doxygen-1.8.11.linux.bin.tar.gz
# popd > /dev/null 2>&1
# -----------------------------------------------------------------------

# Build standalone project and populate archive cache
mkdir -p /work/SGEXT-source
pushd /work/SGEXT-source > /dev/null 2>&1
  cmake -DSGEXTPythonPackage_BUILD_PYTHON:PATH=0 -G Ninja ../
  ninja
popd > /dev/null 2>&1

SINGLE_WHEEL=0

# Compile wheels re-using standalone project and archive cache
for PYBIN in "${PYBINARIES[@]}"; do
    export Python3_EXECUTABLE=${PYBIN}/python3
    Python3_INCLUDE_DIR=$( find -L ${PYBIN}/../include/ -name Python.h -exec dirname {} \; )

    echo ""
    echo "Python3_EXECUTABLE:${Python3_EXECUTABLE}"
    echo "Python3_INCLUDE_DIR:${Python3_INCLUDE_DIR}"

    # Install dependencies
    ${PYBIN}/pip install --upgrade -r /work/requirements-dev.txt

    build_type=MinSizeRel
    source_path=/work/SGEXT-source/SGEXT
    build_path=/work/SGEXT-$(basename $(dirname ${PYBIN}))-manylinux1_${ARCH}
    # SETUP_PY_CONFIGURE="${script_dir}/../setup_py_configure.py"
    SKBUILD_CMAKE_INSTALL_PREFIX=$(${Python3_EXECUTABLE} -c "from skbuild.constants import CMAKE_INSTALL_DIR; print(CMAKE_INSTALL_DIR)")

    # Clean up previous invocations
    rm -rf ${build_path}

    echo "#"
    echo "# Build single SGEXT wheel"
    echo "#"

    # Configure setup.py
    # ${PYBIN}/python ${SETUP_PY_CONFIGURE} "sgext"
    # Generate wheel
    ${PYBIN}/python setup.py bdist_wheel --build-type ${build_type} -G Ninja -- \
          -DSGEXT_SOURCE_DIR:PATH=${source_path} \
          -DSGEXT_BINARY_DIR:PATH=${build_path} \
          -DSGEXTPythonPackage_SGEXT_BINARY_REUSE:BOOL=OFF \
          -DSGEXTPythonPackage_WHEEL_NAME:STRING="sgext" \
          -DSGEXT_WRAP_unsigned_short:BOOL=ON \
          -DSGEXT_WRAP_double:BOOL=ON \
          -DCMAKE_CXX_COMPILER_TARGET:STRING=$(uname -p)-linux-gnu \
          -DPython3_EXECUTABLE:FILEPATH=${Python3_EXECUTABLE} \
          -DPython3_INCLUDE_DIR:PATH=${Python3_INCLUDE_DIR} \
          -DSGEXT_WRAP_DOC:BOOL=ON \
          -DDOXYGEN_EXECUTABLE:FILEPATH=/work/tools/doxygen-1.8.11/bin/doxygen
    # Cleanup
    ${PYBIN}/python setup.py clean

    # Remove unnecessary files for building against SGEXT
    find ${build_path} -name '*.cpp' -delete -o -name '*.xml' -delete
    rm -rf ${build_path}/Wrapping/Generators/castxml*
    find ${build_path} -name '*.o' -delete

done

# auditwheel contains a regression where it will consume the "sgext" wheels.
/opt/python/cp37-cp37m/bin/pip3 install auditwheel==1.9.0 wheel==0.26.0
# Since there are no external shared libraries to bundle into the wheels
# this step will fixup the wheel switching from 'linux' to 'manylinux1' tag
for whl in dist/*linux_$(uname -p).whl; do
    /opt/python/cp37-cp37m/bin/auditwheel repair ${whl} -w /work/dist/
    rm ${whl}
done

# Install packages and test
for PYBIN in "${PYBINARIES[@]}"; do
    ${PYBIN}/pip install --user numpy
    ${PYBIN}/pip install sgext --user --no-cache-dir --no-index -f /work/dist
    (cd $HOME && ${PYBIN}/python -c 'from sgext import SGEXTCommon;')
    (cd $HOME && ${PYBIN}/python -c 'import sgext; image = sgext.Image[sgext.UC, 2].New()')
    (cd $HOME && ${PYBIN}/python -c 'import sgextConfig; sgextConfig.LazyLoading = False; import sgext;')
    (cd $HOME && ${PYBIN}/python ${script_dir}/../../docs/code/testDriver.py )
done
