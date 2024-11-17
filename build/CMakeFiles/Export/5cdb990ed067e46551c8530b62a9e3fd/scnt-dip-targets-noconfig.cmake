#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "dip" for configuration ""
set_property(TARGET dip APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(dip PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/bin/dip"
  )

list(APPEND _cmake_import_check_targets dip )
list(APPEND _cmake_import_check_files_for_dip "${_IMPORT_PREFIX}/bin/dip" )

# Import target "dip-cpp" for configuration ""
set_property(TARGET dip-cpp APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(dip-cpp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/scnt-dip/libdip-cpp.a"
  )

list(APPEND _cmake_import_check_targets dip-cpp )
list(APPEND _cmake_import_check_files_for_dip-cpp "${_IMPORT_PREFIX}/lib/scnt-dip/libdip-cpp.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
