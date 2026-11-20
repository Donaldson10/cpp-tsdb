FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add new test files to this list as you create them.
add_executable(tsdb_tests
  smoke_test.cpp
  series_test.cpp
)
target_link_libraries(tsdb_tests PRIVATE tsdb tsdb_options GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tsdb_tests)
