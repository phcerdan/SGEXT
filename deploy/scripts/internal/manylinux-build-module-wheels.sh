#!/usr/bin/env bash

# -----------------------------------------------------------------------
# These variables are set in common script:
#
ARCH=""
PYBINARIES=""

script_dir=$(cd $(dirname $0) || exit 1; pwd)
source "${script_dir}/manylinux-build-common.sh"
# -----------------------------------------------------------------------

# Compile wheels re-using standalone project and archive cache
for PYBIN in "${PYBINARIES[@]}"; do
    Python3_EXECUTABLE=${PYBIN}/python
    Python3_INCLUDE_DIR=$( find -L ${PYBIN}/../include/ -name Python.h -exec dirname {} \; )
    Python3_INCLUDE_DIRS=${Python3_INCLUDE_DIR}

    echo ""
    echo "Python3_EXECUTABLE:${Python3_EXECUTABLE}"
    echo "Python3_INCLUDE_DIR:${Python3_INCLUDE_DIR}"

    if [[ -e /work/requirements-dev.txt ]]; then
      ${PYBIN}/pip install --upgrade -r /work/requirements-dev.txt
    fi
    version=$(basename $(dirname ${PYBIN}))
    # Remove "m" -- not present in Python 3.8 and later
    version=${version:0:9}
    sgext_build_dir=/work/$(basename SGEXT-${version}*-manylinux1_${ARCH})
    ln -fs SGEXT-${version}*-manylinux1_${ARCH} $sgext_build_dir
    if [[ ! -d ${sgext_build_dir} ]]; then
      echo 'SGEXT build tree not available!' 1>&2
      exit 1
    fi
    sgext_source_dir=/work/SGEXT-source/SGEXT
    ln -fs /SGEXTPythonPackage/SGEXT-source/ /work/SGEXT-source
    if [[ ! -d ${sgext_source_dir} ]]; then
      echo 'SGEXT source tree not available!' 1>&2
      exit 1
    fi
    ${PYBIN}/python setup.py bdist_wheel --build-type Release -G Ninja -- \
      -DSGEXT_DIR:PATH=${sgext_build_dir} \
      -DWRAP_SGEXT_INSTALL_COMPONENT_IDENTIFIER:STRING=PythonWheel \
      -DCMAKE_CXX_COMPILER_TARGET:STRING=$(uname -p)-linux-gnu \
      -DBUILD_TESTING:BOOL=OFF \
      -DPython3_EXECUTABLE:FILEPATH=${Python3_EXECUTABLE} \
      -DPython3_INCLUDE_DIRS:PATH=${Python3_INCLUDE_DIRS} \
    || exit 1
    ${PYBIN}/python setup.py clean
done

# Since there are no external shared libraries to bundle into the wheels
# this step will fixup the wheel switching from 'linux' to 'manylinux1' tag
for whl in dist/*linux_$(uname -p).whl; do
    auditwheel repair ${whl} -w /work/dist/
    rm ${whl}
done
