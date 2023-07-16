# SWANN: Searching With Approximate Nearest Neighbor

Welcome to the SWANN repository! This project focuses on developing a highly efficient indexing and retrieval system for high-dimensional binary vectors using Locality-Sensitive Hashing (LSH). It incorporates trie-based indexing and efficient bucket distribution techniques to enhance search performance.

## Report

The [project report](SWANN_Big-ANN-Challenge_Bachelor_Thesis.tex.pdf) provides a comprehensive study of our LSH algorithm, covering its theory, implementation, and performance analysis. It explores fundamental LSH concepts such as hash families, bucket distributions, and failure probabilities.

## Running

If you want to compile and run:

```$ docker compose up```

If you want to only compile write:

```$ docker compose up compile```

If you want to only compile and test write:

```$ docker compose up test```

To run benchmarking:

```$ docker compose up benchmark```
