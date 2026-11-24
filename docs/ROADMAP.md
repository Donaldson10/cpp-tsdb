# Roadmap

Each milestone should end with working code, tests, and a line in the README.
Ship v0.1 before starting the stretch goals: a small, finished, tested database
is worth more on a resume than a big unfinished one.

## v0.1 - In-memory core
- [x] Decide the data model: what is a "point"? (timestamp, value, series name/tags?)
- [x] Insert points into one series, in memory
- [x] Query a time range from one series
- [x] Unit tests for insert + range query, including edge cases (empty, out-of-order, duplicates)

## v0.2 - Persistence
- [x] Write points to disk (pick a simple file format and document it in DESIGN.md)
- [x] Read them back after a restart
- [x] Write-ahead log (WAL) so a crash doesn't lose recent writes (`Database`: log first, then memory; replay on startup)
- [x] Test: data survives a restart (`DatabaseTest`)
- [x] Test: restart doesn't grow the WAL (replay must not re-log)
- [x] Test: torn final record is skipped on recovery
- [x] Fix: truncate a torn tail when opening the WAL (test: insert after a crash, then restart)
- [ ] Follow-up (optional): WAL test for an unopenable path (`EXPECT_THROW`)

## v0.3 - Compression
- [ ] Delta-of-delta encoding for timestamps
- [ ] XOR encoding for float values (Gorilla paper)
- [ ] Benchmark: bytes per point before vs. after

## v0.4 - Many series + queries
- [ ] Multiple series, identified by name + tags
- [ ] Index from series name/tags to data
- [ ] Aggregations over a range: min, max, avg, count, sum
- [ ] Downsampling (e.g. 1-minute averages)

## v0.5 - Make it usable
- [ ] Command-line tool: insert, query
- [ ] Benchmarks in the README (ingest points/sec, query latency)
- [ ] Ingest real data from the ESP32 weather station

## Stretch goals
- [ ] Retention: delete data older than N days
- [ ] Concurrency: safe reads while writing
- [ ] Simple network API (HTTP or TCP line protocol)
- [ ] Compaction / LSM-style merge of on-disk files
