#include <benchmark/benchmark.h>
#include <iostream>
#include <chrono>
#include <random>
#include <ctime>

#include "../dataset/load.hpp"
#include "../statistics/statsgenerator.hpp"
#include "../index/index.hpp"
#include "../index/bfindex.hpp"
#include "../index/lshforest.hpp"
#include "../index/lshmapfactory.hpp"
#include "../index/lshmap.hpp"
#include "../index/bucketmask.hpp"
#include "../hash/hashpool.hpp"
#include "../hash/dependenthashfamilyfactory.hpp"

static void BM_bucket_distribution(benchmark::State &state) {
  // Setup
  PointsDataset<D> dataset = load_hdf5<D>(static_cast<DataSize>(state.range(0)));

  HashFamily<D> pool = HashFamilyFactory<D>::createRandomBits(D);

  const float P1 = 0.86;
  const float P2 = 0.535;

  const ui32 depth = std::min(
    std::ceil(log((float)dataset.size()) / log(1.0 / P2)),
    30.0
  );

  BucketMask masks(depth);

  // Measure
  for (auto _ : state)
  {
    auto maps = state.range(1) == 0
                ? LSHMapFactory<D>::create(pool, masks, depth, 1)
                : LSHMapFactory<D>::create_optimized(dataset, pool, depth, 1, state.range(1));

    maps[0]->add(dataset);

    auto analyzer = new LSHMapAnalyzer<D>(maps[0]);

    auto dist = analyzer->getBucketDistribution();

    state.counters["nonEmptyBuckets"] = dist.size();
    state.counters["bucketsArrayFactor"] = (float) dist.size() / std::pow(2.0, depth);
    state.counters["largestBucket"] = dist[0];
    state.counters["largestOfTotal"] = (float) dist[0] / (float)dataset.size();
    state.counters["depth"] = depth;
    state.counters["size"] = dataset.size();
    state.counters["optimization"] = state.range(1);
  }


}

// BENCHMARK(BM_bucket_distribution)
//     ->Name("BucketDistribution")
//     ->Unit(benchmark::kMillisecond)

//     ->Args({0, 0})
//     ->Args({0, 1})
//     ->Args({0, 2})
//     ->Args({0, 4})
//     ->Args({0, 8})
//     ->Args({0, 16})

//     ->Args({1, 0})
//     ->Args({1, 1})
//     ->Args({1, 2})
//     ->Args({1, 4})
//     ->Args({1, 8})
//     ->Args({1, 16})
//     ->Repetitions(3)
//     ->DisplayAggregatesOnly(true);

// ->Args({2, 0})
// ->Args({2, 1})
// ->Args({2, 2})
// ->Args({2, 4})
// ->Args({2, 8})
// ->Args({2, 16});