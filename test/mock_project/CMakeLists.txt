cmake_minimum_required(VERSION 3.1)
project(mock_project_using_SGEXT)
# mock/test project to test SGEXTConfig.cmake
# You will still need to provide all the SGEXT dependencies:
# -DGtal_DIR=/path -DITK_DIR=/path -DVTK_DIR and -DBoost_DIR
find_package(SGEXT CONFIG REQUIRED)
add_executable(mock_main mock_main.cpp)
target_link_libraries(mock_main SGEXT::SGAnalyze)

