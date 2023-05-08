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
#include "../hash/hashpool.hpp"
#include "../hash/dependenthashfamilyfactory.hpp"

static void BM_bf_query_10_points_BFIndex(benchmark::State &state)
{
  std::cout << "Loading benchmark dataset" << std::endl;

  // Setup
  BenchmarkDataset<D> dataset = load_benchmark_dataset<D>(static_cast<DataSize>(state.range(0)));

  Index<D> *index = new BFIndex<D>(dataset.points);

  index->build();

  double recalls = 0.0;
  double queriesLength = (double)dataset.queries.size();

  std::cout << "Running brute force query" << std::endl;

  // Measure
  ui32 i = 1;
  for (auto _ : state)
  {
    for (auto &q : dataset.queries)
    {

      if (i % (dataset.queries.size() / 100) == 0)
      {
        std::cout << "Brute force " << ((100 * i) / dataset.queries.size()) << "\% complete" << std::endl;
      }

      // Query
      auto start = std::chrono::high_resolution_clock::now();

      auto result = index->query(q.query, 100, 0.8);

      auto end = std::chrono::high_resolution_clock::now();

      std::transform(ALL(result), result.begin(), [&index, &q](ui32 i)
                     { return q.query.distance((*index)[i]); });
      recalls += calculateRecall(result, q.nearest_neighbors);

      // Save result
      auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
      state.SetIterationTime(elapsed_seconds.count());

      i++;
    }
  }

  recalls /= queriesLength;

  state.counters["recall"] = recalls; // Expected to be 1.0 since brute force
}

/**
 * @brief Benchmark the query performance of the LSHForest index for random bits concat hash family
 */
static void BM_query_x_points_LSHForest(benchmark::State &state)
{
  srand(time(NULL));

  // Setup
  std::cout << "Loading benchmark dataset" << std::endl;
  BenchmarkDataset<D> dataset = load_benchmark_dataset<D>(static_cast<DataSize>(state.range(0)));
  // HashFamily<D> pool = HashFamilyFactory<D>::createRandomBitsConcat(D);


  std::cout << "Instantiating hash LSHMap" << std::endl;
  HashFamily<D> pool = HashFamilyFactory<D>::createRandomBits(D);
  pool += HashFamilyFactory<D>::createRandomBits(D);
  pool += HashFamilyFactory<D>::createRandomBits(D);
  pool += HashFamilyFactory<D>::createRandomBits(D);


  std::cout << "Instantiating maps" << std::endl;
  
  float P1s[] = {0.86, 0.865, 0.87 };
  float P2s[] = {0.5,  0.52,  0.535};

  const float P1 = P1s[state.range(3)];
  const float P2 = P2s[state.range(4)];

  const float depth_val = std::min(
    std::ceil(log(dataset.points.size()) / log(1 / P2)),
    30.0
  );

  const ui32 depth = depth_val;
  const ui32 count = std::ceil(std::pow(P1, -depth_val));


  std::cout << "Depth: " << depth << std::endl
            << "Count: " << count << std::endl
            << "Points: " << dataset.points.size() << std::endl;

  auto maps = LSHMapFactory<D>::create_optimized(dataset.points, pool, depth, count, 8);
  // auto maps = LSHMapFactory<D>::create(pool, depth, count);


  std::cout << "Building index" << std::endl;
  LSHForest<D> *index = new LSHForest<D>(maps, dataset.points, SingleBitFailure<D>);
  index->build();

  std::cout << "Running benchmark" << std::endl;

  double recalls = 0.0, recall = state.range(2) / 100.0, avg_found = 0.0;
  double queriesLength = (double)dataset.queries.size();

  int nrToQuery = state.range(1);

  ui32 i = 1;

  double total_time = 0;

  double slowest_time = 0.0;

  // Measure
  for (auto _ : state)
  {
    for (auto &q : dataset.queries)
    {

      if (i % (dataset.queries.size() / 100) == 0)
      {
        std::cout << "LSHForest " << ((100 * i) / dataset.queries.size()) << "\% complete" << std::endl << std::endl;
      }

      // Query
      auto start = std::chrono::high_resolution_clock::now();
      auto result = index->query(q.query, nrToQuery, recall);
      auto end = std::chrono::high_resolution_clock::now();
      // Save result
      auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

      std::transform(ALL(result), result.begin(), [&index, &q](ui32 i)
                     { return q.query.distance((*index)[i]); });

      recalls += calculateRecall(result, q.nearest_neighbors);

      if (i % (dataset.queries.size() / 100) == 0)
      {
        std::cout << "Stopped at dist " << index->stop_hdist << " with mask index " << index->stop_mask_index << " and a total of " << index->stop_found << " found points" << std::endl;
        std::cout << "Time taken: " << elapsed_time << std::endl;
        std::cout << "Recall: " << calculateRecall(result, q.nearest_neighbors) << std::endl;
        std::cout << "Visited: " << index->buckets_visited << std::endl << std::endl;
      }
      slowest_time = std::max(slowest_time, elapsed_time);
      state.SetIterationTime(elapsed_time);

      avg_found += index->stop_found;
      total_time += elapsed_time;
      i++;
    }
  }
  std::cout << "bucket factor: " << static_cast<LSHForest<D> *>(index)->get_bucket_factor(recall) << std::endl;
  std::cout << ((double)avg_found / queriesLength) << std::endl;
  recalls /= queriesLength;
  state.counters["recall"] = recalls;
  state.counters["kNN"] = nrToQuery;
  state.counters["P1"] = P1;
  state.counters["P2"] = P2;
  state.counters["trie_depth"] = depth;
  state.counters["trie_count"] = count;
  state.counters["timePerQuery"] = (double)total_time / queriesLength;
  state.counters["slowestQuery"] = slowest_time;
  state.counters["foundPerQuery"] = (double)avg_found / queriesLength;


  std::cout << "Querying for " << dataset.queries.size() << " points " << std::endl;

  // // Print bucket distribution
  // std::cout << "Bucket distribution:" << std::endl;
  // StatsGenerator<D>::runStats((LSHForest<D> *)index);
}

/**
 * @brief Benchmark the query performance of the LSHForest index when run on the dependent
 *        hamming distance hash family
 */
static void BM_query_x_points_LSHForest_HammingDistanceDependent(benchmark::State &state)
{
  std::cout << "Loading benchmark dataset" << std::endl;

  // Setup
  BenchmarkDataset<D> dataset = load_benchmark_dataset<D>(static_cast<DataSize>(state.range(0)));

  // Max 100_000 sample size to avoid infinity loop
  const ui32 N_Sample = dataset.points.size() / 10 > 100000 ? 100000 : dataset.points.size() / 10;
  ui32 depth = log(dataset.points.size()) + 2;
  ui32 count = 6;

  std::vector<Point<D>> sample_points;
  std::sample(ALL(dataset.points),
              std::back_inserter(sample_points),
              N_Sample,
              std::mt19937{std::random_device{}()});

  std::cout << "Instantiating hash LSHMap" << std::endl;
  HashFamily<D> HF = DependentHashFamilyFactory<D>::createHDist(ALL(sample_points), depth * count);
  auto maps = LSHMapFactory<D>::create(HF, depth, count);

  std::cout << "Building index" << std::endl;
  Index<D> *index = new LSHForest<D>(maps, dataset.points, HammingSizeFailure);
  index->build();

  std::cout << "Running benchmark" << std::endl;

  double recalls = 0.0,
         queriesLength = (double)dataset.queries.size(), total_time = 0;
  int nrToQuery = state.range(1);

  ui32 i = 1;

  // Measure
  for (auto _ : state)
  {
    for (auto &q : dataset.queries)
    {

      if (i % (dataset.queries.size() / 100) == 0)
      {
        std::cout << "HammingDist_LSHForest " << ((100 * i) / dataset.queries.size()) << "\% complete" << std::endl;
      }

      // Query
      auto start = std::chrono::high_resolution_clock::now();
      auto result = index->query(q.query, nrToQuery, 0.8);
      auto end = std::chrono::high_resolution_clock::now();

      std::transform(ALL(result), result.begin(), [&index, &q](ui32 i)
                     { return q.query.distance((*index)[i]); });
      recalls += calculateRecall(result, q.nearest_neighbors);

      // Save result
      auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
      state.SetIterationTime(elapsed_time);
      total_time += elapsed_time;
      i++;
    }
  }

  recalls /= queriesLength;

  state.counters["recall"] = recalls;
  state.counters["kNN"] = nrToQuery;
  state.counters["timePerQuery"] = (double)total_time / queriesLength;
}

// BENCHMARK(BM_query_x_points_LSHForest_HammingDistanceDependent)
//     ->Name("Query10Points_LSHForest_HammingDistanceDependent")
//     ->Unit(benchmark::kMillisecond)
//     ->Args({0, 10}) // XS
//     ->UseManualTime();

BENCHMARK(BM_query_x_points_LSHForest)
    ->Name("QueryXPointsLSHForest")
    ->Unit(benchmark::kMillisecond)

    ->Args({0, 10, 90, 0, 0}) // XS - query for 10 points
    ->Args({0, 10, 90, 0, 1}) // XS - query for 10 points
    ->Args({0, 10, 90, 0, 2}) // XS - query for 10 points

    ->Args({0, 10, 90, 1, 0}) // XS - query for 10 points
    ->Args({0, 10, 90, 1, 1}) // XS - query for 10 points
    ->Args({0, 10, 90, 1, 2}) // XS - query for 10 points

    ->Args({0, 10, 90, 2, 0}) // XS - query for 10 points
    ->Args({0, 10, 90, 2, 1}) // XS - query for 10 points
    ->Args({0, 10, 90, 2, 2}) // XS - query for 10 points


    ->Args({1, 10, 90, 0, 0}) // S - query for 10 points
    ->Args({1, 10, 90, 0, 1}) // S - query for 10 points
    ->Args({1, 10, 90, 0, 2}) // S - query for 10 points

    ->Args({1, 10, 90, 1, 0}) // S - query for 10 points
    ->Args({1, 10, 90, 1, 1}) // S - query for 10 points
    ->Args({1, 10, 90, 1, 2}) // S - query for 10 points

    ->Args({1, 10, 90, 2, 0}) // S - query for 10 points
    ->Args({1, 10, 90, 2, 1}) // S - query for 10 points
    ->Args({1, 10, 90, 2, 2}) // S - query for 10 points


    ->Args({2, 10, 90, 0, 0}) // M - query for 10 points
    ->Args({2, 10, 90, 0, 1}) // M - query for 10 points
    ->Args({2, 10, 90, 0, 2}) // M - query for 10 points

    ->Args({2, 10, 90, 1, 0}) // M - query for 10 points
    ->Args({2, 10, 90, 1, 1}) // M - query for 10 points
    ->Args({2, 10, 90, 1, 2}) // M - query for 10 points

    ->Args({2, 10, 90, 2, 0}) // M - query for 10 points
    ->Args({2, 10, 90, 2, 1}) // M - query for 10 points
    ->Args({2, 10, 90, 2, 2}) // M - query for 10 points

    ->UseManualTime();
// ->Complexity(benchmark::oN);;
