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

set(_Boost_components Boost::program_options Boost::filesystem)

if(SG_MODULE_LOCATE)
    add_executable(graph_to_vtp graph_to_vtp.cpp)
    target_link_libraries(graph_to_vtp SGLocate )
    target_link_libraries(graph_to_vtp ${_Boost_components} )
    target_link_libraries(graph_to_vtp ${VTK_LIBRARIES} )
endif()

add_executable(fill_holes_binary fill_holes_binary.cpp)
target_link_libraries(fill_holes_binary ${_Boost_components} )
target_link_libraries(fill_holes_binary ${ITK_LIBRARIES} )
if(SG_MODULE_VISUALIZE_WITH_QT)
    target_compile_definitions(fill_holes_binary PUBLIC VISUALIZE)
endif()

add_executable(thin thin.cpp thin_function.cpp)
target_link_libraries(thin SGCore)
target_link_libraries(thin ${_Boost_components} )
target_link_libraries(thin ${ITK_LIBRARIES} )
target_link_libraries(thin ${DGTAL_LIBRARIES} )
target_include_directories(thin PRIVATE ${DGTAL_INCLUDE_DIRS})
if(SG_MODULE_VISUALIZE)
    target_link_libraries(thin SGVisualize )
    target_compile_definitions(thin PUBLIC SG_MODULE_VISUALIZE_ENABLED)
    if(SG_MODULE_VISUALIZE_WITH_QT)
        target_compile_definitions(thin PUBLIC SG_MODULE_VISUALIZE_ENABLED_WITH_QT)
        target_compile_definitions(thin PRIVATE VISUALIZE)
    endif()
endif()

if(SG_MODULE_COMPARE)
    add_executable(merge_low_high_info_graphs merge_low_high_info_graphs.cpp)
    target_link_libraries(merge_low_high_info_graphs SGCompare )
    target_link_libraries(merge_low_high_info_graphs ${_Boost_components})
    if(SG_MODULE_VISUALIZE_WITH_QT)
        target_link_libraries(merge_low_high_info_graphs SGVisualize )
        target_compile_definitions(merge_low_high_info_graphs PUBLIC VISUALIZE)
    endif()
endif()

if(SG_MODULE_ANALYZE)
    add_executable(analyze_graph analyze_graph.cpp analyze_graph_function.cpp)
    target_link_libraries(analyze_graph SGExtract SGAnalyze)
    target_link_libraries(analyze_graph SGAnalyze)
    target_link_libraries(analyze_graph ${_Boost_components})
    target_link_libraries(analyze_graph ${ITK_LIBRARIES} )
    target_link_libraries(analyze_graph ${DGTAL_LIBRARIES} )
    target_include_directories(analyze_graph PRIVATE ${DGTAL_INCLUDE_DIRS} )
    if(SG_MODULE_VISUALIZE)
        target_link_libraries(analyze_graph SGVisualize)
        target_compile_definitions(analyze_graph PUBLIC SG_MODULE_VISUALIZE_ENABLED)
        if(SG_MODULE_VISUALIZE_WITH_QT)
            target_compile_definitions(analyze_graph PUBLIC SG_MODULE_VISUALIZE_ENABLED_WITH_QT)
        endif()
    endif()
endif()

if(SG_MODULE_ANALYZE)
    add_executable(full_graph full_graph.cpp)
    target_link_libraries(full_graph SGExtract SGAnalyze)
    target_link_libraries(full_graph ${_Boost_components})
    target_link_libraries(full_graph
        ${ITK_LIBRARIES}
        ${DGTAL_LIBRARIES}
        )
    target_include_directories(full_graph PRIVATE ${DGTAL_INCLUDE_DIRS})
    if(SG_MODULE_VISUALIZE_WITH_QT)
        target_link_libraries(full_graph SGVisualize )
        # target_link_libraries(full_graph Qt5::Widgets Qt5::Xml )
        target_compile_definitions(full_graph PUBLIC VISUALIZE)
    endif()
endif()

if(SG_MODULE_ANALYZE)
    add_executable(histograms_from_data histograms_from_data.cpp)
    target_link_libraries(histograms_from_data SGAnalyze )
    target_link_libraries(histograms_from_data ${_Boost_components})
endif()

add_executable(create_distance_map create_distance_map.cpp create_distance_map_function.cpp)
target_link_libraries(create_distance_map SGCore )
target_link_libraries(create_distance_map ${_Boost_components})
target_link_libraries(create_distance_map ${ITK_LIBRARIES} )
target_link_libraries(create_distance_map ${DGTAL_LIBRARIES} )
target_include_directories(create_distance_map PRIVATE ${DGTAL_INCLUDE_DIRS} )

add_executable(mask_distance_map_with_thin_image mask_distance_map_with_thin_image.cpp)
target_link_libraries(mask_distance_map_with_thin_image SGCore )
target_link_libraries(mask_distance_map_with_thin_image ${_Boost_components} )
target_link_libraries(mask_distance_map_with_thin_image ${ITK_LIBRARIES} )

if(SG_MODULE_VISUALIZE_WITH_QT AND SG_MODULE_ANALYZE)
    add_executable(highlight_image_points highlight_image_points.cpp)
    target_link_libraries(highlight_image_points ${_Boost_components} )
    target_link_libraries(highlight_image_points ${ITK_LIBRARIES} )
    target_link_libraries(highlight_image_points ${DGTAL_LIBRARIES} )
    target_link_libraries(highlight_image_points Qt5::Widgets Qt5::Xml )
    target_link_libraries(highlight_image_points ${VTK_LIBRARIES} )
    target_include_directories(highlight_image_points PRIVATE ${DGTAL_INCLUDE_DIRS} )

    add_executable(visualize_histograms_from_file
        visualize_histograms_from_file.cpp
        # ${SG_SRC_WITH_VTK_AND_QT}
        )
    target_link_libraries(visualize_histograms_from_file ${_Boost_components})
    target_link_libraries(visualize_histograms_from_file SGAnalyze SGVisualize )
    target_link_libraries(visualize_histograms_from_file ${ITK_LIBRARIES} )
    target_link_libraries(visualize_histograms_from_file Qt5::Widgets Qt5::Xml )
endif()


if(${_modern_VTK_available})
    if(SG_MODULE_VISUALIZE)
        add_executable(visualize_graph_and_image visualize_graph_and_image.cpp)
        target_link_libraries(visualize_graph_and_image SGVisualize)
        target_link_libraries(visualize_graph_and_image ${_Boost_components})
        target_link_libraries(visualize_graph_and_image ${ITK_LIBRARIES} )
        # target_link_libraries(visualize_graph_and_image Qt5::Widgets Qt5::Xml )
    endif()
endif()

if(SG_MODULE_VISUALIZE)
    list(APPEND _scripts_targets ${_scripts_targets_visualize})
endif()

if(SG_MODULE_VISUALIZE_WITH_QT)
    list(APPEND _scripts_targets ${_scripts_targets_visualize_using_qt})
endif()


# Even if you install the targets, the dependencies are not bundled/packaged
# The user would have to set LD_LIBRARY_PATH poiting to the dependency libraries
# DGtal, VTK, etc
# install(TARGETS ${_scripts_targets}
#         RUNTIME DESTINATION bin
#         )
