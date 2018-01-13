cmake_minimum_required(VERSION 3.5)
# cmake_policy(SET CMP0012 NEW)
# if (UNIX)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -pedantic -Wextra")
# endif()
# cmake ../src  -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE
project(dgtal-play)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS
        program_options
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

include_directories("./include")

add_executable(thin thin.cpp)
target_link_libraries(thin ${Boost_LIBRARIES} )
target_link_libraries(thin ${ITK_LIBRARIES} )
target_link_libraries(thin ${DGTAL_LIBRARIES} )
target_link_libraries(thin Qt5::Widgets Qt5::Xml )

enable_testing()
add_subdirectory(test)
