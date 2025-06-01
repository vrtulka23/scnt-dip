message("Building library: dip-cpp")

file(GLOB source_files "./*.cpp" "./**/*.cpp")
add_library(dip-cpp STATIC ${source_files})
target_compile_definitions(dip-cpp PRIVATE)
target_link_libraries(dip-cpp PRIVATE puq-cpp)
