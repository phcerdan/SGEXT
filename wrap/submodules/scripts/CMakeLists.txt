set(module_name_ scripts)
set(module_path_ ${CMAKE_CURRENT_SOURCE_DIR})
set(current_sources_
  sgscripts_init_py.cpp
  thin_py.cpp
  create_distance_map_py.cpp
  reconstruct_from_distance_map_py.cpp
  )

if(SG_MODULE_ANALYZE)
  list(APPEND current_sources_ analyze_graph_py.cpp)
endif()
if(SG_MODULE_VISUALIZE AND SG_MODULE_LOCATE)
  list(APPEND current_sources_ visualize_spatial_graph_py.cpp)
endif()
list(TRANSFORM current_sources_ PREPEND "${module_path_}/")

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  PARENT_SCOPE)

