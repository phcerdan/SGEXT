cmake_minimum_required(VERSION 3.1)
project(mock_project_using_SGEXT)
# mock/test project to test SGEXTConfig.cmake
find_package(SGEXT CONFIG REQUIRED)
add_executable(mock_main mock_main.cpp)
target_link_libraries(mock_main SGEXT::SGAnalyze)

