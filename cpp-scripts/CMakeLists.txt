set(_scripts_targets
    thin
    analyze_graph
    full_graph
    histograms_from_data
    create_distance_map
    mask_distance_map_with_thin_image
    )

set(_scripts_targets_visualize
    highlight_image_points
    visualize_histograms_from_file
    )

add_executable(thin thin.cpp)
target_link_libraries(thin ${Boost_LIBRARIES} )
target_link_libraries(thin ${ITK_LIBRARIES} )
target_link_libraries(thin ${DGTAL_LIBRARIES} )
if(VISUALIZE)
    target_link_libraries(thin Qt5::Widgets Qt5::Xml )
    target_link_libraries(thin ${VTK_LIBRARIES} )
    target_compile_definitions(thin PUBLIC VISUALIZE)
endif()

add_executable(analyze_graph analyze_graph.cpp)
target_link_libraries(analyze_graph SG )
target_link_libraries(analyze_graph ${Boost_LIBRARIES} )
target_link_libraries(analyze_graph ${ITK_LIBRARIES} )
target_link_libraries(analyze_graph ${DGTAL_LIBRARIES} )
if(VISUALIZE)
    target_link_libraries(analyze_graph Qt5::Widgets Qt5::Xml )
    target_link_libraries(analyze_graph SGVtk )
    # target_link_libraries(analyze_graph ${VTK_LIBRARIES} )
    target_compile_definitions(analyze_graph PUBLIC VISUALIZE)
endif()

add_executable(full_graph full_graph.cpp)
target_link_libraries(full_graph SG )
target_link_libraries(full_graph ${Boost_LIBRARIES} )
target_link_libraries(full_graph ${ITK_LIBRARIES} )
target_link_libraries(full_graph ${DGTAL_LIBRARIES} )
if(VISUALIZE)
    target_link_libraries(full_graph Qt5::Widgets Qt5::Xml )
    target_link_libraries(full_graph SGVtk )
    # target_link_libraries(full_graph ${VTK_LIBRARIES} )
    target_compile_definitions(full_graph PUBLIC VISUALIZE)
endif()

add_executable(histograms_from_data histograms_from_data.cpp)
target_link_libraries(histograms_from_data SG )
target_link_libraries(histograms_from_data ${Boost_LIBRARIES} )

add_executable(create_distance_map create_distance_map.cpp)
target_link_libraries(create_distance_map SG )
target_link_libraries(create_distance_map ${Boost_LIBRARIES} )
target_link_libraries(create_distance_map ${ITK_LIBRARIES} )
target_link_libraries(create_distance_map ${DGTAL_LIBRARIES} )

add_executable(mask_distance_map_with_thin_image mask_distance_map_with_thin_image.cpp)
target_link_libraries(mask_distance_map_with_thin_image SG )
target_link_libraries(mask_distance_map_with_thin_image ${Boost_LIBRARIES} )
target_link_libraries(mask_distance_map_with_thin_image ${ITK_LIBRARIES} )

if(VISUALIZE)
    add_executable(highlight_image_points highlight_image_points.cpp)
    target_link_libraries(highlight_image_points ${Boost_LIBRARIES} )
    target_link_libraries(highlight_image_points ${ITK_LIBRARIES} )
    target_link_libraries(highlight_image_points ${DGTAL_LIBRARIES} )
    target_link_libraries(highlight_image_points Qt5::Widgets Qt5::Xml )
    target_link_libraries(highlight_image_points ${VTK_LIBRARIES} )

    add_executable(visualize_histograms_from_file
      visualize_histograms_from_file.cpp
      ${SG_SRC_WITH_VTK_AND_QT}
      )
    target_link_libraries(visualize_histograms_from_file SG )
    target_link_libraries(visualize_histograms_from_file ${Boost_LIBRARIES} )
    target_link_libraries(visualize_histograms_from_file ${ITK_LIBRARIES} )
    target_link_libraries(visualize_histograms_from_file ${DGTAL_LIBRARIES} )
    target_link_libraries(visualize_histograms_from_file Qt5::Widgets Qt5::Xml )
    target_link_libraries(visualize_histograms_from_file ${VTK_LIBRARIES} )
endif()

if(VISUALIZE)
    list(APPEND _scripts_targets ${_scripts_targets_visualize})
endif()

# Even if you install the targets, the dependencies are not bundled/packaged
# The user would have to set LD_LIBRARY_PATH poiting to the dependency libraries
# DGtal, VTK, etc
install(TARGETS ${_scripts_targets}
        RUNTIME DESTINATION bin
        )
