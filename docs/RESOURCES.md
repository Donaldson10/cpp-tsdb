# Reading list

Read in this order. You don't need to finish a resource before you start coding: read
enough to build the next milestone in [ROADMAP.md](ROADMAP.md), then come back.

## 1. Start here (read before writing code)

- **"Writing a Time Series Database from Scratch"** by Fabian Reinartz, who wrote Prometheus's storage engine.
  The best single introduction: why time-series data is different and how the storage is laid out.
  https://fabxc.org/tsdb/ (if the site is down, use the archived copy:
  https://web.archive.org/web/2024/https://fabxc.org/tsdb/)
- **Gorilla paper** (Facebook, VLDB 2015): "Gorilla: A Fast, Scalable, In-Memory Time Series Database".
  Read section 4.1 (compression) closely; you'll implement it in v0.3.
  https://www.vldb.org/pvldb/vol8/p1816-teller.pdf

## 2. Database fundamentals

- **CMU 15-445/645 Intro to Database Systems** (Andy Pavlo). Free lectures on YouTube, slides on the site.
  Watch the storage, buffer pool and indexing lectures first.
  https://15445.courses.cs.cmu.edu/
- **_Designing Data-Intensive Applications_** by Martin Kleppmann. Chapter 3 ("Storage and Retrieval")
  explains logs, SSTables, LSM trees and B-trees. Your library may have it, or O'Reilly via the CSUMB library.
- **_Database Internals_** by Alex Petrov. Part I is about storage engines. Use it as a reference, not cover to cover.

## 3. How real time-series databases do it

- **Prometheus TSDB internals** by Ganesh Vernekar: a blog series on the head block, WAL, checkpoints and
  on-disk blocks. https://ganeshvernekar.com/blog/prometheus-tsdb-the-head-block/
- **Prometheus storage docs**: short overview of blocks, WAL and retention.
  https://prometheus.io/docs/prometheus/latest/storage/
- **Your own capstone.** You already know TimescaleDB from the user side (hypertables, chunks, continuous
  aggregates). Compare its design with yours in DESIGN.md; that is a strong interview story.

## 4. C++ references

- **learncpp.com**: refresh on anything rusty (classes, move semantics, smart pointers). https://www.learncpp.com/
- **cppreference.com**: the standard library reference. Bookmark it. https://en.cppreference.com/
- **GoogleTest primer**: how to write tests. https://google.github.io/googletest/primer.html
- **Google Benchmark user guide**: how to write benchmarks. https://github.com/google/benchmark/blob/main/docs/user_guide.md
- **CMake**: you shouldn't need much more than this repo already has. When you add a file, add it to the
  list in `CMakeLists.txt` (or `tests/CMakeLists.txt`).

## 5. Optional, for later milestones

- **"Build Your Own Database From Scratch"** by James Smith (build-your-own.org): a step-by-step book on a
  B-tree storage engine, persistence and fsync. Useful for v0.2. https://build-your-own.org/
- **LSM trees**: search "LSM tree explained" and read the RocksDB wiki overview when you get to compaction.
- **Endianness and binary file formats**: search "C++ write binary file portable endianness" before designing
  your on-disk format.

## How to find what you need when you're stuck

1. **Name the problem in database terms.** "My data is lost when the program crashes" becomes
   "write-ahead log" or "fsync durability". The right term gets much better search results.
2. **See how a real system did it.** Search "<problem> Prometheus TSDB", "InfluxDB TSM", or "VictoriaMetrics".
   Their design docs and blog posts explain the tradeoffs.
3. **Go to the primary source.** Papers (Gorilla), official docs (cppreference) and the authors' own blogs
   beat random tutorials.
4. **Write a tiny experiment first.** Before building a feature into the database, test the idea in a
   20-line test in `tests/`.
5. **Write down what you decided** in DESIGN.md's decision log, including what you tried that didn't work.
