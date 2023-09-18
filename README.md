# SWANN: Searching With Approximate Nearest Neighbor

Welcome to the SWANN repository! This project focuses on developing a highly efficient indexing and retrieval system for high-dimensional binary vectors using Locality-Sensitive Hashing (LSH). It incorporates trie-based indexing and efficient bucket distribution techniques to enhance search performance.

## Example: Run a single query

This basic example covers loading, building and querying of the LSH index.

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
    HashFamily<D> pool = HashFamilyFactory<D>::createRandomBits();
    
    // Instantiate maps
    const uint32 steps = 8;
    auto maps = LSHMapFactory<D>::create_optimized(
        dataset,
        pool,
        depth,
        count,
        steps
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
    auto result = index->query(
        q,
        nns,
        recall
    );
}
```

## Example: Run multiple queries

This example covers the fully optimized loading, building and querying of a list of queries. It uses multithreading to speed up the process.

```c++
int main() {
  
    // Load dataset
    vector<bitset<D>> dataset = ...;
    vector<bitset<D>> queries = ...;

    // results[i] : indices of the 10 nearest neighbours to queries[i]
    std::vector<std::vector<ui32>> results;

    // Specify hyperparameters
    const float depth_val = std::min(
        std::ceil(log(dataset.size()) / log(1 / P2)),
        30.0
    );
    // Probabilities of collision 
    const float P1 = 0.860f, P2 = 0.535f;   
    
    // Precision of the query
    const float recall = 0.9f;              
    
    // Number of nearest neighbors to query
    const ui32 nrToQuery = 10;              
    
    // Depth of the trie, we recommend the above value
    const ui32 depth = depth_val;           
    
    // Number of LSH Tries to use in the forest
    const ui32 count = std::ceil(std::pow(P1, -depth_val)); 
    
    // Number of optimizations steps to perform  
    const uint32 steps = 20;                

    // Instantiate hash family
    HashFamily<D> pool = HashFamilyFactory<D>::createRandomBits(D*4);
    
    // Instantiate maps
    auto maps = LSHMapFactory<D>::mthread_create_optimized(
        dataset,
        pool,
        depth,
        count,
        steps
    );

    // Create and build index
    LSHForest<D> *index = new LSHForest<D>(
        maps,
        dataset,
        SingleBitFailure<D>
    );
    index->build();

    // Run queries simoultaneously
    results = index->mthread_queries(
        queries,
        nrToQuery,
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
