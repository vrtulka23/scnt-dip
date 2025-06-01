message("Compiling gtests: TestSolver")

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
enable_testing()

file(GLOB test_files "*.cpp")
add_executable(TestSolver ${test_files})
target_link_libraries(TestSolver PRIVATE GTest::gtest_main dip-cpp)

include(GoogleTest)
gtest_discover_tests(TestSolver)
