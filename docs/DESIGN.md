# Design notes


## Data model
A **point** is a `(timestamp, value)` pair.

- **Timestamp:** `int64_t`, milliseconds since the Unix epoch (`using Timestamp = int64_t;`).
  Sensor readings arrive every few seconds, so ms precision is plenty. Integers compare fast and
  compress well with delta-of-delta encoding (Gorilla, v0.3).
- **Value:** `double`. Covers every sensor reading (temperature, humidity, pressure), and Gorilla's
  XOR encoding is designed for doubles.
- **Range queries are half-open `[start, end)`:** a point at `start` is included, a point at `end` is not.
  Back-to-back windows (1:00-2:00, 2:00-3:00) never double-count a point, and it matches the C++
  standard library convention (`begin`/`end`, `std::lower_bound`).
- **Duplicate timestamps: last write wins.** Inserting a point at an existing timestamp overwrites the
  old value, so a series holds at most one value per timestamp. This matches Prometheus/InfluxDB and
  handles sensor retries gracefully.
- **Series identity:** v0.1 has a single series; naming (name -> Series map) comes next.

## Write path
v0.1 (in-memory): points live in a `std::vector<Point>` kept sorted by timestamp (the class invariant).
1. Binary search (`std::lower_bound`) for the first point with timestamp >= the new one.
2. If that position holds the same timestamp, overwrite its value (last write wins).
3. Otherwise insert the new point at that position, which keeps the vector sorted.

Cost: O(log n) to find the position, plus O(n) worst case to shift later elements on insert.
In practice sensor data arrives nearly in order, so most inserts land at the end and are cheap.

## On-disk format
_File layout, block size, header fields, endianness. Draw it._

## Read / query path
v0.1: `query(start, end)` returns a copy of all points in `[start, end)`.
1. If `start >= end`, return an empty result (an empty interval contains no points).
2. Two binary searches with `std::lower_bound`: one for `start`, one for `end` (half-open, so `end` excluded).
3. Copy the range between the two positions into the result vector.

Cost: O(log n + k), where k is the number of points returned.

## Compression
_Which encodings, and measured results._

## Durability
_WAL format, when it's flushed (fsync), how recovery works._

## Decision log

| Date | Decision | Alternatives considered | Why |
|------|----------|-------------------------|-----|
| 2026-11-19 | Timestamps are `int64_t` ms since epoch | `std::chrono::time_point`; seconds; ns | Simple, fast comparisons, compresses well; ms is enough for sensor data |
| 2026-11-19 | Values are `double` | ints too; multiple value types | Covers all sensor data; Gorilla XOR targets doubles |
| 2026-11-19 | Range queries are half-open `[start, end)` | Inclusive `[start, end]` | No double-counting across adjacent windows; matches STL conventions |
| 2026-11-19 | Duplicate timestamps: last write wins | Keep both; reject with error | One value per timestamp is easy to reason about; matches Prometheus/InfluxDB; tolerates retries |
| 2026-11-19 | Query with `start >= end` returns empty | Throw `std::invalid_argument` | An empty interval contains no points; consistent with `start == end`; callers never crash on bad input |
| 2026-11-19 | Store points in a sorted `std::vector` | `std::map`; unsorted vector + sort on query | Contiguous memory is cache-friendly; binary search gives O(log n) lookups; simplest structure for v0.1 |
