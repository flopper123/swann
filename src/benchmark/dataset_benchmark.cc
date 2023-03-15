
#include <benchmark/benchmark.h>
#include <iostream>
#include "../dataset/load.hpp"
#include "../index/index.hpp"
#include "../index/lshforest.hpp"
#include "../index/lshmapfactory.hpp"
#include "../hash/hashpool.hpp"

constexpr ui32 TRIE_DEPTH = 22, TRIE_COUNT = 5;

static void BM_run_XS_dataset(benchmark::State &state) {
  std::cout << "Loading benchmark dataset" << std::endl;
  // Setup
  BenchmarkDataset<D> dataset = load_benchmark_dataset<D>(DataSize::XS);

  std::cout << "Instantiating hash LSHMap" << std::endl;
  HashFamily<D> pool = HashFamilyFactory<D>::createRandomBits(D);
  auto maps = LSHMapFactory<D>::create(pool, TRIE_DEPTH, TRIE_COUNT);
  
  std::cout << "Building index" << std::endl;
  Index<D> *index = new LSHForest<D>(maps, dataset.points);
  index->build();
  
  std::cout << "Running benchmark" << std::endl;
  
  double recalls;
  double queriesLength = (double)dataset.queries.size();

  // Measure
  for (auto _ : state) {
    for (auto &q : dataset.queries) {
      auto result = index->query(q.query, 10, 0.8);
      
      recalls += calculateRecall(result, q.nearest_neighbors);
    }
  }

  recalls /= queriesLength;

  state.counters["recall"] = recalls;

}


// Add to benchmarks
BENCHMARK(BM_run_XS_dataset)->Name("QueryXSBenchmarkDataset")->Unit(benchmark::kMillisecond);
