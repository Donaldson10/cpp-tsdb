# tsdb

A time-series database written from scratch in C++20.

> **Status:** early development. See [docs/ROADMAP.md](docs/ROADMAP.md) for progress.

<!-- Fill these sections in as you build. Recruiters read the first screen of a README,
     so keep the top short: what it is, what works, one number that shows it works. -->

## What it does

_TODO: one paragraph. What data goes in, what queries come out, what makes it a time-series database._

## Features

- [ ] _TODO: list features as they land (e.g. append-only writes, time-range queries, compression)_

## Design

_TODO: 3-5 sentences plus a diagram. Full details in [docs/DESIGN.md](docs/DESIGN.md)._

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
