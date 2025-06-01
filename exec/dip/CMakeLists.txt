message("Compiling executable: dip")

file(GLOB example_files "*.cpp" "**/*.cpp")
add_executable(dip ${example_files} )
target_compile_definitions(dip PRIVATE CODE_VERSION="${CODE_VERSION}")
target_link_libraries(dip PRIVATE dip-cpp)
