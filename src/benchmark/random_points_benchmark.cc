#include <benchmark/benchmark.h>
#include "../dataset/load.hpp"
#include "../index/bfindex.hpp"
#include "../index/index.hpp"
#include "../index/lshforest.hpp"
#include "../index/lshmapfactory.hpp"
#include "../hash/hashpool.hpp"

constexpr ui32 TRIE_DEPTH = 3, TRIE_COUNT = 3;

Index<D> *q_index, *bf_index;
std::vector<Point<D>> queries;
HashFamily<D> pool = *new HashPool<D>();

static void BM_run_XS_random_points(benchmark::State &state) {  
  // Measure
  for (auto _ : state) {
    for (auto &q : queries) {
      q_index->query(q, 10, 0.8);
    }
  }
}

static void BM_run_XS_BF(benchmark::State &state) {
  // Measure
  for (auto _ : state) {
    for (auto &q : queries) {
      bf_index->query(q, 10, 0.8);
    }
  }
}

// Setup function
static void DoSetupRand(const benchmark::State &state) {
  
  Dataset<Points<D>> in = load_hdf5(DataSize::XS);
  auto maps = LSHMapFactory<D>::create(pool, TRIE_DEPTH, TRIE_COUNT);
  q_index = new LSHForest<D>(maps, in);
  q_index->build();
  
  // Build brute-force index
  bf_index = new BFIndex<D>(in);
  bf_index->build();

  // Generate queries at random
  const ui32 N = 100;
  for (int i=0; i < N; ++i) {
    queries.emplace_back(in[rand() % in.size()]);
  }
}

// Add to benchmarks
BENCHMARK(BM_run_XS_random_points)->Name("QueryXSRandomPoints")->Unit(benchmark::kMillisecond)->Setup(DoSetupRand);
BENCHMARK(BM_run_XS_BF)->Name("QueryXSRandomPoints_BF")->Unit(benchmark::kMillisecond)->Setup(DoSetupRand);
