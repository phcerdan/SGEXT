set(_scripts_targets
    thin
    analyze_graph
    full_graph
    histograms_from_data
    create_distance_map
    mask_distance_map_with_thin_image
    merge_low_high_info_graphs
    fill_holes_binary
    graph_to_vtp
    )

set(_scripts_targets_visualize # Using VTK
    visualize_graph_and_image
    )

set(_scripts_targets_visualize_using_qt # Using Qt (DGtal or VTK)
    highlight_image_points
    visualize_histograms_from_file
    )

set(VISUALIZE_USING_QT 0)
set(VISUALIZE_USING_VTK 0)
if(SG_MODULE_VISUALIZE AND VISUALIZE_ENABLE_QT)
    set(VISUALIZE_USING_QT 1)
endif()
if(SG_MODULE_VISUALIZE AND NOT VISUALIZE_ENABLE_QT)
    set(VISUALIZE_USING_VTK 1)
endif()

add_executable(graph_to_vtp graph_to_vtp.cpp)
target_link_libraries(graph_to_vtp SGLocate )
target_link_libraries(graph_to_vtp ${Boost_LIBRARIES} )
target_link_libraries(graph_to_vtp ${VTK_LIBRARIES} )

add_executable(fill_holes_binary fill_holes_binary.cpp)
target_link_libraries(fill_holes_binary ${Boost_LIBRARIES} )
target_link_libraries(fill_holes_binary ${ITK_LIBRARIES} )
if(VISUALIZE_USING_QT)
    target_compile_definitions(fill_holes_binary PUBLIC VISUALIZE)
endif()

add_executable(thin thin.cpp)
target_link_libraries(thin ${Boost_LIBRARIES} )
target_link_libraries(thin ${ITK_LIBRARIES} )
target_link_libraries(thin ${DGTAL_LIBRARIES} )
target_include_directories(thin PRIVATE ${DGTAL_INCLUDE_DIRS})
if(VISUALIZE_USING_QT)
    # target_link_libraries(thin Qt5::Widgets Qt5::Xml )
    target_link_libraries(thin ${VTK_LIBRARIES} )
    target_compile_definitions(thin PUBLIC VISUALIZE)
endif()

if(SG_MODULE_COMPARE)
    add_executable(merge_low_high_info_graphs merge_low_high_info_graphs.cpp)
    target_link_libraries(merge_low_high_info_graphs SGCompare )
    if(VISUALIZE_USING_QT)
        target_link_libraries(merge_low_high_info_graphs SGVisualize )
        target_compile_definitions(merge_low_high_info_graphs PUBLIC VISUALIZE)
    endif()
endif()

add_executable(analyze_graph analyze_graph.cpp)
target_link_libraries(analyze_graph SGExtract SGAnalyze )
target_link_libraries(analyze_graph ${ITK_LIBRARIES} )
target_link_libraries(analyze_graph ${DGTAL_LIBRARIES} )
target_include_directories(analyze_graph PRIVATE ${DGTAL_INCLUDE_DIRS} )
if(VISUALIZE_USING_QT)
    target_link_libraries(analyze_graph SGVisualize )
    # target_link_libraries(analyze_graph Qt5::Widgets Qt5::Xml )
    target_compile_definitions(analyze_graph PUBLIC VISUALIZE)
endif()

add_executable(full_graph full_graph.cpp)
target_link_libraries(full_graph SGExtract SGAnalyze)
target_link_libraries(full_graph
    ${ITK_LIBRARIES}
    ${DGTAL_LIBRARIES}
    )
target_include_directories(full_graph PRIVATE ${DGTAL_INCLUDE_DIRS})
if(VISUALIZE_USING_QT)
    target_link_libraries(full_graph SGVisualize )
    # target_link_libraries(full_graph Qt5::Widgets Qt5::Xml )
    target_compile_definitions(full_graph PUBLIC VISUALIZE)
endif()

if(SG_MODULE_ANALYZE)
    add_executable(histograms_from_data histograms_from_data.cpp)
    target_link_libraries(histograms_from_data SGAnalyze )
endif()

add_executable(create_distance_map create_distance_map.cpp)
target_link_libraries(create_distance_map SGCore )
target_link_libraries(create_distance_map ${ITK_LIBRARIES} )
target_link_libraries(create_distance_map ${DGTAL_LIBRARIES} )
target_include_directories(create_distance_map PRIVATE ${DGTAL_INCLUDE_DIRS} )

add_executable(mask_distance_map_with_thin_image mask_distance_map_with_thin_image.cpp)
target_link_libraries(mask_distance_map_with_thin_image SGCore )
target_link_libraries(mask_distance_map_with_thin_image ${Boost_LIBRARIES} )
target_link_libraries(mask_distance_map_with_thin_image ${ITK_LIBRARIES} )

if(VISUALIZE_USING_QT)
    add_executable(highlight_image_points highlight_image_points.cpp)
    target_link_libraries(highlight_image_points ${Boost_LIBRARIES} )
    target_link_libraries(highlight_image_points ${ITK_LIBRARIES} )
    target_link_libraries(highlight_image_points ${DGTAL_LIBRARIES} )
    target_link_libraries(highlight_image_points Qt5::Widgets Qt5::Xml )
    target_link_libraries(highlight_image_points ${VTK_LIBRARIES} )
    target_include_directories(highlight_image_points PRIVATE ${DGTAL_INCLUDE_DIRS} )

    add_executable(visualize_histograms_from_file
        visualize_histograms_from_file.cpp
        # ${SG_SRC_WITH_VTK_AND_QT}
        )
    target_link_libraries(visualize_histograms_from_file SGAnalyze SGVisualize )
    target_link_libraries(visualize_histograms_from_file ${ITK_LIBRARIES} )
    target_link_libraries(visualize_histograms_from_file Qt5::Widgets Qt5::Xml )
endif()


if(${_modern_VTK_available})
    if(VISUALIZE_USING_VTK)
        add_executable(visualize_graph_and_image visualize_graph_and_image.cpp)
        target_link_libraries(visualize_graph_and_image SGVisualize)
        target_link_libraries(visualize_graph_and_image ${ITK_LIBRARIES} )
        # target_link_libraries(visualize_graph_and_image Qt5::Widgets Qt5::Xml )
    endif()
endif()

if(VISUALIZE_USING_VTK)
    list(APPEND _scripts_targets ${_scripts_targets_visualize})
endif()

if(VISUALIZE_USING_QT)
    list(APPEND _scripts_targets ${_scripts_targets_visualize_using_qt})
endif()


# Even if you install the targets, the dependencies are not bundled/packaged
# The user would have to set LD_LIBRARY_PATH poiting to the dependency libraries
# DGtal, VTK, etc
# install(TARGETS ${_scripts_targets}
#         RUNTIME DESTINATION bin
#         )
