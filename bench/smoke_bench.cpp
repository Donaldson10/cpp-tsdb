#include <benchmark/benchmark.h>

#include "tsdb/version.hpp"

// Proves the benchmark setup works. Replace with real benchmarks
// (ingest points/sec, query latency, bytes per point) as you build.
static void BM_Version(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(tsdb::version());
  }
}
BENCHMARK(BM_Version);
