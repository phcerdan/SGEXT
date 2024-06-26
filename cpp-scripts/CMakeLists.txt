set(_scripts_targets
    thin
    analyze_graph
    histograms_from_data
    create_distance_map
    mask_distance_map_with_thin_image
    merge_low_high_info_graphs
    fill_holes_binary
    resample_image
    make_isotropic
    graph_to_vtp
    voxelize_graph
    )

set(_scripts_targets_visualize # Using VTK
    visualize_graph_and_image
    visualize_with_label_image
    )

set(_scripts_targets_visualize_using_qt # Using Qt (DGtal or VTK)
    highlight_image_points
    visualize_histograms_from_file
    )

set(_Boost_components Boost::program_options Boost::filesystem)
set(_scripts_libs
    SGScripts
    ${_Boost_components}
    )

if(SG_MODULE_LOCATE)
    add_executable(graph_to_vtp graph_to_vtp.cpp)
    target_link_libraries(graph_to_vtp ${_scripts_libs})
endif()

add_executable(fill_holes_binary fill_holes_binary.cpp)
target_link_libraries(fill_holes_binary ${_scripts_libs})
if(SG_MODULE_VISUALIZE_WITH_QT)
    target_compile_definitions(fill_holes_binary PUBLIC VISUALIZE)
endif()

add_executable(resample_image resample_image.cpp)
target_link_libraries(resample_image ${_scripts_libs})

add_executable(make_isotropic make_isotropic.cpp)
target_link_libraries(make_isotropic ${_scripts_libs})

add_executable(voxelize_graph voxelize_graph.cpp)
target_link_libraries(voxelize_graph ${_scripts_libs})

add_executable(thin thin.cpp)
target_link_libraries(thin ${_scripts_libs})

if(SG_MODULE_COMPARE)
    add_executable(merge_low_high_info_graphs merge_low_high_info_graphs.cpp)
    target_link_libraries(merge_low_high_info_graphs ${_scripts_libs})
    if(SG_MODULE_VISUALIZE_WITH_QT)
        target_compile_definitions(merge_low_high_info_graphs PUBLIC VISUALIZE)
    endif()
endif()

if(SG_MODULE_ANALYZE)
    add_executable(analyze_graph analyze_graph.cpp)
    target_link_libraries(analyze_graph ${_scripts_libs})
    if(SG_MODULE_VISUALIZE)
        target_compile_definitions(analyze_graph PUBLIC SG_MODULE_VISUALIZE_ENABLED)
        if(SG_MODULE_VISUALIZE_WITH_QT)
            target_compile_definitions(analyze_graph PUBLIC SG_MODULE_VISUALIZE_ENABLED_WITH_QT)
        endif()
    endif()
endif()

if(SG_MODULE_ANALYZE)
    add_executable(histograms_from_data histograms_from_data.cpp)
    target_link_libraries(histograms_from_data SGAnalyze )
    target_link_libraries(histograms_from_data ${_Boost_components})
endif()

add_executable(create_distance_map create_distance_map.cpp)
target_link_libraries(create_distance_map ${_scripts_libs} )

add_executable(mask_distance_map_with_thin_image mask_distance_map_with_thin_image.cpp)
target_link_libraries(mask_distance_map_with_thin_image ${_scripts_libs} )

if(SG_MODULE_VISUALIZE_WITH_QT AND SG_MODULE_ANALYZE)
    add_executable(highlight_image_points highlight_image_points.cpp)
    target_link_libraries(highlight_image_points ${_scripts_libs})
    target_link_libraries(highlight_image_points Qt5::Widgets Qt5::Xml )

    add_executable(visualize_histograms_from_file visualize_histograms_from_file.cpp)
    target_link_libraries(visualize_histograms_from_file ${_scripts_libs})
    target_link_libraries(visualize_histograms_from_file Qt5::Widgets Qt5::Xml )
endif()

# Because: volumeMapper->SetBlendModeToIsoSurface
if(_modern_VTK_available)
    if(SG_MODULE_VISUALIZE)
        add_executable(visualize_graph_and_image visualize_graph_and_image.cpp)
        target_link_libraries(visualize_graph_and_image ${_scripts_libs})

        add_executable(render_binary_volume render_binary_volume.cpp)
        target_link_libraries(render_binary_volume ${_scripts_libs})

        vtk_module_autoinit(
            TARGETS visualize_graph_and_image render_binary_volume
            MODULES ${VTK_LIBRARIES})
    endif()
endif()

if(SG_MODULE_VISUALIZE)
    add_executable(view_image_with_label visualize_image_with_label.cpp)
    target_link_libraries(view_image_with_label ${_scripts_libs})
    if(_modern_VTK_available)
        vtk_module_autoinit(
            TARGETS view_image_with_label
            MODULES ${VTK_LIBRARIES})
    endif()
endif()

# Even if you install the targets, the dependencies are not bundled/packaged
# The user would have to set LD_LIBRARY_PATH poiting to the dependency libraries
# DGtal, VTK, etc
# install(TARGETS ${_scripts_targets}
#         RUNTIME DESTINATION bin
#         )
