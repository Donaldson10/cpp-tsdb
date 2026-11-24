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
v0.2: a write-ahead log (WAL). Every insert is appended to a log file; on startup the log is replayed
to rebuild the in-memory series.

- **Record format:** fixed-size 16-byte binary records, back to back: 8-byte timestamp (`int64_t`)
  followed by 8-byte value (`double`). Fixed size means no separators are needed: record N starts at
  byte N * 16. (Variable-size records would need a length prefix; not needed yet.)
- **File location:** the `Wal` class takes the file path as a constructor parameter. Tests pass their own
  temp files; the CLI uses a default path when none is given.
- **When writes reach the file:** every insert is appended and flushed immediately. `flush()` hands the
  data to the OS, so it survives the *program* crashing. It does not survive power loss or an OS crash;
  that needs `fsync` (planned, slower).
- **Missing file:** treated as an empty log (a brand new database), and created on open. Failing to
  open or create the file (bad folder, no permission) is an error and throws.
- **Recovery:** read 16-byte records until end of file. The log is append-only, so a crash can only damage
  the last record. When the WAL is opened, if the file size is not a multiple of 16 bytes, the torn
  tail is **truncated** (`std::filesystem::resize_file`) back to the last complete record, before any
  new appends. Replay then only ever sees complete records.
  - *Why truncate instead of just ignoring it:* the first version ignored the torn bytes on replay but
    left them in the file. The next append then landed after the garbage, shifting every later record
    by 8 bytes: replay produced a fake point (torn timestamp + the next record's timestamp bytes read as
    a double) and lost the real one. Found by a test that inserts after a simulated crash.
  - Planned: per-record checksums to detect records that are complete but corrupted.
- **Log growth:** the WAL is never truncated (beyond the torn tail), because it is the only copy of the
  data on disk. Overwritten duplicates stay in the file. Trimming waits until snapshots exist (v0.3).
- **Byte order:** records use the machine's native byte order (little-endian on x86 and Apple Silicon).
  Fine for one machine; a portable format would fix the byte order explicitly.

### Database: tying WAL and Series together
- `Database` owns a `WAL` and a `Series` (composition, not inheritance, so the only way to insert is
  through `Database::insert`, which always logs).
- **Insert:** append to the WAL first, then insert into the series ("write-ahead": if the program dies
  between the two, the point is still on disk and is recovered on restart).
- **Startup:** the constructor replays the WAL straight into the series, *not* through
  `Database::insert`, otherwise every restart would re-log every point and double the file.

## Decision log

| Date | Decision | Alternatives considered | Why |
|------|----------|-------------------------|-----|
| 2026-11-19 | Timestamps are `int64_t` ms since epoch | `std::chrono::time_point`; seconds; ns | Simple, fast comparisons, compresses well; ms is enough for sensor data |
| 2026-11-19 | Values are `double` | ints too; multiple value types | Covers all sensor data; Gorilla XOR targets doubles |
| 2026-11-19 | Range queries are half-open `[start, end)` | Inclusive `[start, end]` | No double-counting across adjacent windows; matches STL conventions |
| 2026-11-19 | Duplicate timestamps: last write wins | Keep both; reject with error | One value per timestamp is easy to reason about; matches Prometheus/InfluxDB; tolerates retries |
| 2026-11-19 | Query with `start >= end` returns empty | Throw `std::invalid_argument` | An empty interval contains no points; consistent with `start == end`; callers never crash on bad input |
| 2026-11-19 | Store points in a sorted `std::vector` | `std::map`; unsorted vector + sort on query | Contiguous memory is cache-friendly; binary search gives O(log n) lookups; simplest structure for v0.1 |
| 2026-11-20 | WAL records are fixed 16-byte binary (timestamp + value) | Text/CSV; length-prefixed variable records | Smaller and faster than text; fixed size makes record boundaries implicit; length prefix not needed until records vary |
| 2026-11-20 | WAL path is a constructor parameter; CLI has a default | Hard-coded fixed path | Tests need isolated temp files and can run in parallel; keeps real data separate from test data |
| 2026-11-20 | Flush the WAL after every insert | Batch writes; `fsync` every insert | Survives program crashes with little cost; `fsync` (power-loss safety) deferred as a known trade-off |
| 2026-11-20 | Missing WAL file = empty log, created on open | Treat missing file as an error | A new database has no log yet; append mode creates it. Only failure to open/create is an error |
| 2026-11-20 | On replay, ignore an incomplete final record | Treat it as fatal corruption | Append-only log means only the tail can be torn by a crash; earlier records are intact. *Superseded 2026-11-23, see below* |
| 2026-11-22 | `Database` composes a `WAL` and a `Series` | Inherit from `Series` | Inheritance would expose `Series::insert`, bypassing the log |
| 2026-11-22 | Replay goes directly into the series, not through `Database::insert` | Replay via `insert` | Replaying via `insert` re-logs every point and doubles the file on each restart (caught by `RestartsDoNotGrowWal`) |
| 2026-11-22 | WAL is never trimmed (except torn tail) | Clear the WAL after replay | The WAL is the only on-disk copy; trimming needs snapshots first |
| 2026-11-23 | Truncate a torn tail when the WAL is opened | Ignore it on replay (previous decision) | Ignoring left garbage in the file; the next append misaligned every later record (fake point, lost data). Caught by `InsertAfterCrash` |
