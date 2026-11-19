set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  benchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.9.1.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(benchmark)

# Add new benchmark files to this list as you create them.
add_executable(tsdb_bench
  smoke_bench.cpp
)
target_link_libraries(tsdb_bench PRIVATE tsdb benchmark::benchmark_main)
