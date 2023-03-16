
#include <benchmark/benchmark.h>
#include <iostream>
#include "../dataset/load.hpp"
#include "../index/index.hpp"
#include "../index/bfindex.hpp"
#include "../index/lshforest.hpp"
#include "../index/lshmapfactory.hpp"
#include "../hash/hashpool.hpp"

static void BM_bf_query_10_points_BFIndex(benchmark::State &state)
{
  std::cout << "Loading benchmark dataset" << std::endl;

  // Setup
  BenchmarkDataset<D> dataset = load_benchmark_dataset<D>(static_cast<DataSize>(state.range(0)));

  Index<D> *index = new BFIndex<D>(dataset.points);

  index->build();

  double recalls;
  double queriesLength = (double)dataset.queries.size();

  std::cout << "Running brute force query" << std::endl;
  // Measure
  ui32 i = 1;
  for (auto _ : state)
  {
    for (auto &q : dataset.queries)
    {

      if (i % (dataset.queries.size()/100) == 0) {
        std::cout << "Brute force " << ((100 * i) / dataset.queries.size()) << "\% complete" << std::endl;
      }

      auto result = index->query(q.query, 10, 0.8);

      recalls += calculateRecall(result, q.nearest_neighbors);

      i++;
    }
  }

  recalls /= queriesLength;

  state.counters["recall"] = recalls; // Expected to be 1.0 since brute force
}

/**
 * @brief Benchmark the query performance of the LSHForest index
 */
constexpr ui32 TRIE_DEPTH = 22, TRIE_COUNT = 5;

static void BM_query_10_points_LSHForest(benchmark::State &state) {
  std::cout << "Loading benchmark dataset" << std::endl;
  // Setup
  BenchmarkDataset<D> dataset = load_benchmark_dataset<D>(static_cast<DataSize>(state.range(0)));

  std::cout << "Instantiating hash LSHMap" << std::endl;
  HashFamily<D> pool = HashFamilyFactory<D>::createRandomBits(D);
  auto maps = LSHMapFactory<D>::create(pool, TRIE_DEPTH, TRIE_COUNT);
  
  std::cout << "Building index" << std::endl;
  Index<D> *index = new LSHForest<D>(maps, dataset.points);
  index->build();
  
  std::cout << "Running benchmark" << std::endl;
  
  double recalls;
  double queriesLength = (double)dataset.queries.size();

  ui32 i = 1;

  // Measure
  for (auto _ : state) {
    for (auto &q : dataset.queries) {

      if (i % (dataset.queries.size()/100) == 0) {
        std::cout << "LSHForest " << ((100 * i) / dataset.queries.size()) << "\% complete" << std::endl;
      }

      auto result = index->query(q.query, 10, 0.8);
      
      recalls += calculateRecall(result, q.nearest_neighbors);
      
      i++;
    }
  }

  recalls /= queriesLength;

  state.counters["recall"] = recalls;
}


// Add to benchmarks
BENCHMARK(BM_bf_query_10_points_BFIndex)
    ->Name("QueryBF10PointsBFIndex")
    ->Unit(benchmark::kMillisecond)
    ->Arg(0)  // XS
    ->Arg(1); // S

BENCHMARK(BM_query_10_points_LSHForest)
    ->Name("Query10PointsLSHForest")
    ->Unit(benchmark::kMillisecond)
    ->Arg(0)  // XS
    ->Arg(1); // S
