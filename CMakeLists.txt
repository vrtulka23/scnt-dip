cmake_minimum_required(VERSION 3.22)
cmake_policy(SET CMP0074 NEW)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(DIP VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-deprecated -O3")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(MODULE_NAME scnt-dip)
#set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
set(CMAKE_INSTALL_BINDIR bin)
set(CMAKE_INSTALL_LIBDIR lib)
set(CMAKE_INSTALL_INCLUDEDIR include)

# compile libraries and executables
###################################

message("Code version: ${CODE_VERSION}")

find_package(SCNT-EXS REQUIRED)
find_package(SCNT-PUQ REQUIRED)

message("Building library: dip-cpp")
file(GLOB source_files "src/*.cpp" "src/**/*.cpp")
add_library(dip-cpp STATIC ${source_files})
target_compile_definitions(dip-cpp PRIVATE)
target_link_libraries(dip-cpp PRIVATE puq-cpp)

message("Compiling executable: dip")
file(GLOB example_files "exec/dip/*.cpp" "exec/dip/**/*.cpp")
add_executable(dip ${example_files} )
target_compile_definitions(dip PRIVATE CODE_VERSION="${CODE_VERSION}")
target_link_libraries(dip PRIVATE dip-cpp)

# add Google tests
###################

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
enable_testing()

file(GLOB test_files "gtest/*.cpp")
add_executable(TestSolver ${test_files})
target_link_libraries(TestSolver PRIVATE GTest::gtest_main dip-cpp)

include(GoogleTest)
gtest_discover_tests(TestSolver)

# create cmake export files
###########################

include(CMakePackageConfigHelpers)

configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/${MODULE_NAME}-config.cmake.in
  	"${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}-config.cmake"
  	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${MODULE_NAME}
	PATH_VARS CMAKE_INSTALL_INCLUDEDIR
)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}-config-version.cmake"
    	VERSION ${CODE_VERSION}
    	COMPATIBILITY AnyNewerVersion
)

# install files
###############

install(TARGETS dip EXPORT DipTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(TARGETS dip-cpp EXPORT DipTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/${MODULE_NAME})
	
install(DIRECTORY src/
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${MODULE_NAME}
	FILES_MATCHING PATTERN "*.h")
	
install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}-config-version.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${MODULE_NAME}
)

install(EXPORT DipTargets
        FILE ${MODULE_NAME}-targets.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${MODULE_NAME}"
)

