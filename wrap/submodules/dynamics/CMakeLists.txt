set(module_name_ dynamics)
set(module_path_ ${CMAKE_CURRENT_SOURCE_DIR})
set(files_using_vtk "")
if(SG_REQUIRES_VTK)
  list(APPEND files_using_vtk
    vtu_file_io_py.cpp)
endif()
set(current_sources_
  sgdynamics_init_py.cpp
  ### Data structures
  particle_py.cpp
  bond_py.cpp
  particle_collection_py.cpp
  bond_collection_py.cpp
  system_py.cpp
  ### Forces and Integrator
  forces_py.cpp
  force_compute_py.cpp
  force_compute_with_functional_py.cpp
  integrator_py.cpp
  # Glues
  dynamics_graph_glue_py.cpp
  ${files_using_vtk}
  )
set(wrap_header_${module_name_}
  sgdynamics_common_py.hpp
  )
list(TRANSFORM current_sources_ PREPEND "${module_path_}/")

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  PARENT_SCOPE)

install(FILES ${wrap_header_${module_name_}}
  DESTINATION ${CMAKE_INSTALL_PYTHONINCLUDEDIR}/${module_name_}/
  COMPONENT development)
