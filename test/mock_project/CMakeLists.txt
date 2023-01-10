cmake_minimum_required(VERSION 3.1)
project(mock_project_using_SGEXT)
# mock/test project to test SGEXTConfig.cmake
# You will still need to provide all the SGEXT dependencies:
# -DGtal_DIR=/path -DITK_DIR=/path -DVTK_DIR and -DBoost_DIR
find_package(SGEXT CONFIG REQUIRED)

add_executable(mock_main mock_main.cpp)
target_link_libraries(mock_main SGEXT::SGAnalyze)
option(SGEXT_WITH_WRAP "SGEXT was compiled with python wrappings and this projects want to reuse them" OFF)
if(SGEXT_WITH_WRAP)
  # Pybind11 does not link to Python in Linux and MacOS
  find_package(Python COMPONENTS REQUIRED Interpreter Development)
  target_link_libraries(mock_main SGEXT::_sgext)
  target_link_libraries(mock_main Python::Python)
  target_compile_definitions(mock_main PUBLIC SGEXT_WITH_WRAP)

  # Add pybind11 module (it's a library)
  # Inlude the macro pybind11_add_module (needs SGEXT from a build tree)

  list(APPEND CMAKE_MODULE_PATH "${SGEXT_DIR}/_deps/pybind11-src/tools")
  include(${SGEXT_DIR}/_deps/pybind11-src/tools/pybind11NewTools.cmake)
  pybind11_add_module(_mock
    SHARED
    mock_module_py.cpp # Where module is added
    ${all_modules_python_sources} # all the collected sources in submodules
    )
  add_library(SGEXT::_mock ALIAS _mock)
  # To access headers related with pybind11 defined in sgext, i.e locate/sglocateo_common.h, or pybind11_common.h
  target_link_libraries(_mock PUBLIC SGEXT::_sgext)
  # To access pybind11 headers (might not be needed)
  # target_link_libraries(_mock PRIVATE SGEXT::pybind11)
  # We need to link to python
  target_link_libraries(_mock PRIVATE Python::Python)
endif()

