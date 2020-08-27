set(module_name_ locate)
set(module_path_ ${CMAKE_CURRENT_SOURCE_DIR})
set(current_sources_
  sglocate_init_py.cpp
  get_vtk_points_from_graph_py.cpp
  graph_points_locator_py.cpp
  )
set(wrap_header_${module_name_}
  sglocate_common.h
  )
list(TRANSFORM current_sources_ PREPEND "${module_path_}/")

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  PARENT_SCOPE)

install(FILES ${wrap_header_${module_name_}}
  DESTINATION ${CMAKE_INSTALL_PYTHONINCLUDEDIR}/${module_name_}/
  COMPONENT development)
