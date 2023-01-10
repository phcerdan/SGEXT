# Requires DEPENDENCIES_BUILD_DIR
echo "In deploy script: DEPENDENCIES_BUILD_DIR $DEPENDENCIES_BUILD_DIR"
echo "script_dir: $PSScriptRoot"
echo "$(python --version)"

cd $PSScriptRoot

python -m pip install --upgrade pip
python -m pip install cmake
python -m pip install ninja
python -m pip install -r $PSScriptRoot/requirements-deploy.txt

python setup.py bdist_wheel --build-type Release -G Ninja -- -DCMAKE_C_COMPILER="cl.exe" -DCMAKE_CXX_COMPILER="cl.exe" -DSG_BUILD_TESTING:BOOL=OFF -DSG_MODULE_ANALYZE:BOOL=ON -DSG_MODULE_COMPARE:BOOL=ON -DSG_MODULE_LOCATE:BOOL=ON -DSG_MODULE_GENERATE:BOOL=ON -DSG_MODULE_DYNAMICS:BOOL=ON -DSG_MODULE_VISUALIZE:BOOL=OFF -DSG_MODULE_VISUALIZE_WITH_QT:BOOL=OFF -DSG_WRAP_PYTHON:BOOL=ON -DDEPENDENCIES_BUILD_DIR:PATH=$(DEPENDENCIES_BUILD_DIR)

