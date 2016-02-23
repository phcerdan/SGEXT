cmake_minimum_required(VERSION 2.8.2)
set(CMAKE_ECLIPSE_VERSION=4.4)
# cmake_policy(SET CMP0012 NEW)
if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -pedantic -Wextra -std=c++11")
endif()
# cmake ../src  -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE
project(dgtal-play)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(DGtal REQUIRED)
include_directories(SYSTEM ${DGTAL_INCLUDE_DIRS})
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
include_directories(SYSTEM ${ITK_INCLUDE_DIRS})
find_package(Qt5 REQUIRED
    Widgets
    Xml
    OpenGL)

add_executable(main main.cpp)
target_link_libraries(main ${ITK_LIBRARIES} )
target_link_libraries(main ${DGTAL_LIBRARIES} )
target_link_libraries(main Qt5::Widgets Qt5::Xml )

add_executable(testVoxelThin testVoxelComplexThinning.cpp )
target_link_libraries(testVoxelThin ${ITK_LIBRARIES} )
target_link_libraries(testVoxelThin ${DGTAL_LIBRARIES} )
target_link_libraries(testVoxelThin Qt5::Widgets Qt5::Xml )
