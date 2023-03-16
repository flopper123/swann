#include <benchmark/benchmark.h>
#include <iostream>
#include "../dataset/load.hpp"
#include "../index/index.hpp"
#include "../index/lshforest.hpp"
#include "../index/lshmapfactory.hpp"
#include "../hash/hashpool.hpp"

constexpr ui32 TRIE_DEPTH = 22, TRIE_COUNT = 5;

static void BM_build_LSHForest_index(benchmark::State &state) {
  // Setup
  BenchmarkDataset<D> dataset = load_benchmark_dataset<D>(static_cast<DataSize>(state.range(0)));
  HashFamily<D> pool = HashFamilyFactory<D>::createRandomBits(D);
  auto maps = LSHMapFactory<D>::create(pool, TRIE_DEPTH, TRIE_COUNT);

  Index<D> *index = new LSHForest<D>(maps, dataset.points);

  // Measure
  for (auto _ : state)
  {
    index->build();
  }

  state.counters["recall"] = 0.0;
}

// Add to benchmarks
BENCHMARK(BM_build_LSHForest_index)
    ->Name("BuildLSHForestIndex")
    ->Unit(benchmark::kMillisecond)
    ->Arg(0); // XS
    ->Arg(1); // S
