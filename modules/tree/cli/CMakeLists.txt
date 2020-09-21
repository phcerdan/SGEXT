add_executable(generation_analysis generation_analysis.cpp)

# Boost_DIR is set from SGEXT
find_package(Boost COMPONENTS
        program_options
        REQUIRED)

set(_cli_libs
    SGEXT::SGTree
    SGEXT::SGVisualize
    SGEXT::SGScripts
    Boost::program_options
    )

target_link_libraries(generation_analysis ${_cli_libs})
if(SG_MODULE_VISUALIZE)
  target_compile_definitions(generation_analysis PUBLIC SG_MODULE_VISUALIZE_ENABLED)
endif()
