# SWANN: Searching With Approximate Nearest Neighbor

Welcome to the SWANN repository! This project focuses on developing a highly efficient indexing and retrieval system for high-dimensional binary vectors using Locality-Sensitive Hashing (LSH). It incorporates trie-based indexing and efficient bucket distribution techniques to enhance search performance.


## Basic example
```c++
int main() {
  
    // Load dataset
    vector<bitset<D>> dataset = ...;
    
    // Specify hyperparameters
    const float recall = 0.9f;
    const uint32 depth = ...;
    const uint32 count = ...;
    const uint32 nns = 10;

    // Instantiate hash family
    HashFamily<D> pool = HashFamilyFactory<D>::create_random_bits();
    
    // Instantiate maps
    const uint32 optimization_steps = 8;
    auto maps = LSHMapFactory<D>::create_optimized(
        dataset,
        pool,
        depth,
        count,
        optimization_steps
    );

    // Create and build index
    Index<D> *index = new LSHForest<D>(
        maps,
        dataset,
        SingleBitFailure<D>
    );
    index->build();

    // Run query on query point q
    bitset<D> q = ...;
    auto query_result = index->query(
        q,
        nns,
        recall
    );
}
```


## Report

The [project report](SWANN_Big-ANN-Challenge_Bachelor_Thesis.pdf) provides a comprehensive study of our LSH algorithm, covering its theory, implementation, and performance analysis. It explores fundamental LSH concepts such as hash families, bucket distributions, and failure probabilities.

## Running

If you want to compile and run:

```$ docker compose up```

If you want to only compile write:

```$ docker compose up compile```

If you want to only compile and test write:

```$ docker compose up test```

To run benchmarking:

```$ docker compose up benchmark```
