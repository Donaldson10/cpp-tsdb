# Design notes

Write down every real decision here, with the alternatives you rejected and why.
This file is what you will talk through in interviews ("walk me through a design
decision you made"), so keep it honest, including what didn't work.

## Data model
_What is a point? Timestamp type and precision (seconds, ms, ns)? Value type (double only, or ints too)?
How is a series identified?_

## Write path
_What happens, step by step, when a point is inserted?_

## On-disk format
_File layout, block size, header fields, endianness. Draw it._

## Read / query path
_How does a time-range query find its data? What's the complexity?_

## Compression
_Which encodings, and measured results._

## Durability
_WAL format, when it's flushed (fsync), how recovery works._

## Decision log

| Date | Decision | Alternatives considered | Why |
|------|----------|-------------------------|-----|
|      |          |                         |     |
