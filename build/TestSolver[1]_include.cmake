if(EXISTS "/Users/perseus/Projects/scnt-dip/build/TestSolver[1]_tests.cmake")
  include("/Users/perseus/Projects/scnt-dip/build/TestSolver[1]_tests.cmake")
else()
  add_test(TestSolver_NOT_BUILT TestSolver_NOT_BUILT)
endif()
