# tsdb

A time-series database written from scratch in C++20.

> **Status:** early development. See [docs/ROADMAP.md](docs/ROADMAP.md) for progress.

<!-- Fill these sections in as you build. Recruiters read the first screen of a README,
     so keep the top short: what it is, what works, one number that shows it works. -->

## What it does

tsdb stores **time-stamped measurements**, such as temperature readings from a sensor every few seconds,
and answers **time-range queries** ("all readings between 1:00 and 2:00"). Each point is a 64-bit
millisecond timestamp and a `double` value. Unlike a general-purpose database, it is built around how
time-series data behaves: writes almost always arrive in time order, and reads ask for contiguous ranges
of time. Points are kept sorted in memory for fast range lookups, and a write-ahead log persists them to disk
so data can survive a restart.

## Features

- [x] In-memory series kept sorted by timestamp; out-of-order inserts handled
- [x] Half-open time-range queries `[start, end)` in O(log n + k) using binary search
- [x] Duplicate timestamps: last write wins
- [x] Write-ahead log: fixed 16-byte binary records, flushed on every append, replayable after restart
- [x] `Database` ties them together: every insert is logged first, then applied in memory; startup replays the log
- [x] Crash tolerance: a torn final record is ignored on replay
- [x] Unit tests (GoogleTest), run under AddressSanitizer + UBSan
- [ ] Per-record checksums and `fsync` durability
- [ ] Multiple series with names and tags
- [ ] Gorilla compression (delta-of-delta timestamps, XOR-encoded values)
- [ ] Aggregations and downsampling
- [ ] Command-line tool and real sensor data ingest

## Design

Two components so far: an in-memory `Series` (a `std::vector` kept sorted by timestamp) and a
write-ahead log (`WAL`) on disk. Queries use two binary searches (`std::lower_bound`) to find the range
boundaries and copy out the points in between. The WAL appends each point as a 16-byte record. A `Database` class
owns both: every insert is logged first, then applied in memory, and on startup the log is replayed to
rebuild the series.

```
insert(t, v) ──► WAL file (append + flush) ──► sorted in-memory Series ◄── query(start, end)
                       │                               ▲
                       └──────── replay on startup ────┘
```

Every decision, with the alternatives considered, is in [docs/DESIGN.md](docs/DESIGN.md).

## Benchmarks

_TODO: a small table once real benchmarks exist: machine, dataset, ingest points/sec, query latency, bytes per point._

## Build and run

Requires CMake 3.24+ and a C++20 compiler (Clang 15+, GCC 12+ or MSVC 2022).

```bash
cmake --preset debug            # configure (downloads GoogleTest the first time)
cmake --build --preset debug    # build
ctest --preset debug            # run tests
./build/debug/tsdb_cli          # run the command-line tool
```

Other presets:

| Preset    | Use it for                                                  |
|-----------|-------------------------------------------------------------|
| `debug`   | Everyday development                                         |
| `asan`    | Catching memory bugs (AddressSanitizer + UBSan)              |
| `release` | Benchmarks (`./build/release/bench/tsdb_bench`)              |

## Project layout

```
include/tsdb/   public headers
src/            library code + main.cpp (command-line tool)
tests/          GoogleTest unit tests
bench/          Google Benchmark benchmarks
docs/           design notes, roadmap, reading list
```

## License

MIT, see [LICENSE](LICENSE).
