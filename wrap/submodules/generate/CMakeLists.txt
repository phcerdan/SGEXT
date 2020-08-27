set(module_name_ generate)
set(module_path_ ${CMAKE_CURRENT_SOURCE_DIR})
set(current_sources_
  sggenerate_init_py.cpp
  simulated_annealing_generator_py.cpp
  contour_length_generator_py.cpp
  )
list(TRANSFORM current_sources_ PREPEND "${module_path_}/")

add_subdirectory(histo)

set(all_modules_python_sources
  ${all_modules_python_sources}
  ${current_sources_}
  PARENT_SCOPE)
