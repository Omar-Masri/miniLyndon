# miniLyndon
MiniLyndon is a novel and computationally efficient long-read to long-read overlapper designed for perfect or low-error sequencing reads.

## Pre-requisites
- gcc ≥ 5.1
- GLib ≥ 2.68

## Getting Started

```
git clone https://github.com/Omar-Masri/miniLyndon.git
cd miniLyndon && make
```

# Run

```
./run.sh <directory> <reads.fa> <number of threads>
```

# Run with postprocessing

Postprocessing involves calculating the Hamming distance between overlaps.

```
./run_post.sh <directory> <reads.fa> <number of threads>
```
