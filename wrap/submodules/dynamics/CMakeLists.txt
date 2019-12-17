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
  ${files_using_vtk}
  )
set(module_name_ ${CMAKE_CURRENT_SOURCE_DIR})
list(TRANSFORM current_sources_ PREPEND "${module_name_}/")

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  PARENT_SCOPE)
