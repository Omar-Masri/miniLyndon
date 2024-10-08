# miniLyndon
**miniLyndon** is a novel and computationally efficient long-read to long-read overlapper designed for perfect or low-error sequencing reads.

# Pre-requisites
- **GCC** ≥ 5.1
- **GLib** ≥ 2.68

# Getting Started

```
git clone https://github.com/Omar-Masri/miniLyndon.git
cd miniLyndon && make
```

# Usage

## Basic Run

```
./run.sh <directory> <reads.fa> <number of threads> > <out.paf>
```

- **\<directory\>**: The directory where **\<reads.fa\>** is stored.
- **\<reads.fa\>**: The FASTA file containing the long-reads to be overlapped.
- **\<number of threads\>**: Specifies the number of threads to use for parallel processing.
- **\<out.paf\>**: The output file where the overlaps will be saved in [PAF](#output).


## Run with postprocessing

Postprocessing involves calculating the Hamming distance between overlaps.

```
./run_post.sh <directory> <reads.fa> <number of threads> > <out.paf>
```

This step is crucial for ensuring that the overlaps are suitable for use in downstream assembly processes.


# <a name="output"></a> Output

The output file generated by miniLyndon is in the [PAF](https://github.com/lh3/miniasm/blob/master/PAF.md) (Pairwise Alignment Format), a widely used format in bioinformatics for representing sequence overlaps. Each line in a PAF file corresponds to an alignment between two reads.
