# Roadmap

Each milestone should end with working code, tests, and a line in the README.
Ship v0.1 before starting the stretch goals: a small, finished, tested database
is worth more on a resume than a big unfinished one.

## v0.1 - In-memory core
- [ ] Decide the data model: what is a "point"? (timestamp, value, series name/tags?)
- [ ] Insert points into one series, in memory
- [ ] Query a time range from one series
- [ ] Unit tests for insert + range query, including edge cases (empty, out-of-order, duplicates)

## v0.2 - Persistence
- [ ] Write points to disk (pick a simple file format and document it in DESIGN.md)
- [ ] Read them back after a restart
- [ ] Write-ahead log (WAL) so a crash doesn't lose recent writes
- [ ] Tests that simulate a crash and recover

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
