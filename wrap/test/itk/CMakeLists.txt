if(SG_BUILD_TESTING_INTERACTIVE)
  set(_with_interactive_tests test_itk_view_image.py)
endif()
set(python_tests
  test_itk_image.py
  test_itk_image_io.py
  test_from_to_itk.py
  ${_with_interactive_tests}
  )

get_filename_component(module_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(test_folder "${CMAKE_CURRENT_SOURCE_DIR}")

SG_add_python_tests()
