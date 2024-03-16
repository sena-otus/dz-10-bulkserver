find_package(GTest REQUIRED)

enable_testing()


add_executable(gtest_bulkserver gtest_bulkserver.cpp)

set_target_properties(gtest_bulkserver PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

target_include_directories(gtest_bulkserver
  PRIVATE "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}/src"
)


target_link_libraries(gtest_bulkserver
  PRIVATE
  GTest::gtest_main
  async
)

gtest_discover_tests(gtest_bulkserver)

configure_file(testinput1.txt . COPYONLY)
configure_file(testinput2.txt . COPYONLY)
configure_file(testoutput1.txt . COPYONLY)
configure_file(testoutput2.txt . COPYONLY)
