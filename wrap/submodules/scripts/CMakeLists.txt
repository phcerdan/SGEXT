set(current_sources_
  sgscripts_init_py.cpp
  thin_py.cpp
  create_distance_map_py.cpp
  mask_image_py.cpp
  fill_holes_py.cpp
  )

if(SG_MODULE_ANALYZE)
  list(APPEND current_sources_ analyze_graph_py.cpp)
  set(extra_sources_
    ${CMAKE_SOURCE_DIR}/cpp-scripts/analyze_graph_function.cpp
    )
endif()
set(module_name_ ${CMAKE_CURRENT_SOURCE_DIR})
list(TRANSFORM current_sources_ PREPEND "${module_name_}/")

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  ${CMAKE_SOURCE_DIR}/cpp-scripts/thin_function.cpp
  ${CMAKE_SOURCE_DIR}/cpp-scripts/create_distance_map_function.cpp
  ${CMAKE_SOURCE_DIR}/cpp-scripts/fill_holes_function.cpp
  ${extra_sources_}
  PARENT_SCOPE)

