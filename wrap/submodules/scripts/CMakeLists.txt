set(current_sources_
  sgscripts_init_py.cpp
  analyze_graph_py.cpp
  )
set(module_name_ ${CMAKE_CURRENT_SOURCE_DIR})
list(TRANSFORM current_sources_ PREPEND "${module_name_}/")

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  ${CMAKE_SOURCE_DIR}/cpp-scripts/analyze_graph_function.cpp
  PARENT_SCOPE)
