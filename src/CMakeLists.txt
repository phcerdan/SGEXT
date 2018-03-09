cmake_minimum_required(VERSION 3.5)
# cmake_policy(SET CMP0012 NEW)
# if (UNIX)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -pedantic -Wextra")
# endif()
# cmake ../src  -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE
project(dgtal-play)
option(ENABLE_TESTING "Enable Tests" OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS
        program_options
        filesystem
        graph
        REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
find_package(DGtal REQUIRED)
include_directories(SYSTEM ${DGTAL_INCLUDE_DIRS})
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
include_directories(SYSTEM ${ITK_INCLUDE_DIRS})
find_package(Qt5 REQUIRED
    Widgets
    Xml
    OpenGL)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
include_directories(SYSTEM ${VTK_INCLUDE_DIRS})

include_directories("./include")

add_executable(thin thin.cpp)
target_link_libraries(thin ${Boost_LIBRARIES} )
target_link_libraries(thin ${ITK_LIBRARIES} )
target_link_libraries(thin ${DGTAL_LIBRARIES} )
target_link_libraries(thin Qt5::Widgets Qt5::Xml )
target_link_libraries(thin ${VTK_LIBRARIES} )

add_executable(analyze_graph analyze_graph.cpp)
target_link_libraries(analyze_graph ${Boost_LIBRARIES} )
target_link_libraries(analyze_graph ${ITK_LIBRARIES} )
target_link_libraries(analyze_graph ${DGTAL_LIBRARIES} )
target_link_libraries(analyze_graph Qt5::Widgets Qt5::Xml )
target_link_libraries(analyze_graph ${VTK_LIBRARIES} )

add_executable(highlight_image_points highlight_image_points.cpp)
target_link_libraries(highlight_image_points ${Boost_LIBRARIES} )
target_link_libraries(highlight_image_points ${ITK_LIBRARIES} )
target_link_libraries(highlight_image_points ${DGTAL_LIBRARIES} )
target_link_libraries(highlight_image_points Qt5::Widgets Qt5::Xml )
target_link_libraries(highlight_image_points ${VTK_LIBRARIES} )

if(ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif(ENABLE_TESTING)
