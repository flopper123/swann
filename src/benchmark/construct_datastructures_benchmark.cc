#include <benchmark/benchmark.h>
#include <iostream>
#include "../dataset/load.hpp"
#include "../index/index.hpp"
#include "../index/lshforest.hpp"
#include "../index/lshmapfactory.hpp"
#include "../hash/hashpool.hpp"

/**
 * @brief Benchmark the performance of building a LSH Forest index with multiple threads
 * @param state { DataSize, OptimizationSteps, P1, P2 } 
 */
static void BM_rebuild_mthread(benchmark::State &state) {
  srand(time(NULL));

  const DataSize sz = static_cast<DataSize>(state.range(0));
  const ui32 optimization_steps = state.range(1),
             thread_cnt = std::thread::hardware_concurrency();
  const float P1 = state.range(2) / 1000.0;
  const float P2 = state.range(3) / 1000.0;
  
  // Setup
  BenchmarkDataset<D> dataset = load_benchmark_dataset<D>(sz);

  HashFamily<D> pool = HashFamilyFactory<D>::createRandomBits(D);
  pool += HashFamilyFactory<D>::createRandomBits(D);
  pool += HashFamilyFactory<D>::createRandomBits(D);
  pool += HashFamilyFactory<D>::createRandomBits(D);

  const float depth_val = std::min(
    std::ceil(log(dataset.points.size()) / log(1 / P2)),
    30.0
  );

  const ui32 depth = depth_val;
  const ui32 count = std::ceil(std::pow(P1, -depth_val));

  std::cout << "<Started Benchmark of single threaded rebuild>" << std::endl
            << "Depth: " << depth << std::endl
            << "Count: " << count << std::endl
            << "Points: " << dataset.points.size() << std::endl;

  auto total_time_rebuild = 0.0,
       total_time_index = 0.0;

  // Measure
  for (auto _ : state)
  {
    // Query
    std::cout << "Building " << count << " LSH-Tries with " << optimization_steps << " repetitions" << std::endl;

    auto start_rebuild = std::chrono::high_resolution_clock::now();
    auto maps = LSHMapFactory<D>::mthread_create_optimized(dataset.points, pool, depth, count, optimization_steps);
    auto end_rebuild = std::chrono::high_resolution_clock::now();
    auto time_rebuild = std::chrono::duration_cast<std::chrono::duration<double>>(end_rebuild - start_rebuild).count(); 
    total_time_rebuild += time_rebuild;
    
    auto start_index = std::chrono::high_resolution_clock::now();
    LSHForest<D> *index = new LSHForest<D>(maps, dataset.points, SingleBitFailure<D>);
    index->build();
    auto end_index = std::chrono::high_resolution_clock::now();
    auto time_index = std::chrono::duration_cast<std::chrono::duration<double>>(end_index - start_index).count(); 
    total_time_index += time_index;

    state.SetIterationTime(time_rebuild + time_index);
  }
  
  auto sz_str = DATA_SIZES_LIST[sz];
  state.counters["input_size"] = DATA_SIZES_VALUE_LIST[sz];  
  state.counters["optimization_steps"] = optimization_steps;
  state.counters["thread_cnt"] = thread_cnt;
  state.counters["P1"] = P1;
  state.counters["P2"] = P1;
  state.counters["trie_depth"] = depth;
  state.counters["trie_count"] = count;
  state.counters["total_time_rebuild"] = total_time_rebuild;
  state.counters["total_time_index"] = total_time_index;
  state.counters["total_time"] = total_time_rebuild + total_time_index;
}

/**
 * @brief Benchmark the performance of building a LSH Forest index with a single thread
 * @param state { DataSize, OptimizationSteps, P1, P2 } 
 */
static void BM_rebuild(benchmark::State &state) {
  srand(time(NULL));

  const DataSize sz = static_cast<DataSize>(state.range(0));
  const ui32 optimization_steps = state.range(1);
  const float P1 = state.range(2) / 1000.0;
  const float P2 = state.range(3) / 1000.0;
  
  // Setup
  BenchmarkDataset<D> dataset = load_benchmark_dataset<D>(sz);

  HashFamily<D> pool = HashFamilyFactory<D>::createRandomBits(D);
  pool += HashFamilyFactory<D>::createRandomBits(D);
  pool += HashFamilyFactory<D>::createRandomBits(D);
  pool += HashFamilyFactory<D>::createRandomBits(D);


  const float depth_val = std::min(
    std::ceil(log(dataset.points.size()) / log(1 / P2)),
    30.0
  );

  const ui32 depth = depth_val;
  const ui32 count = std::ceil(std::pow(P1, -depth_val));

  std::cout << "<Started Benchmark of single threaded rebuild>" << std::endl
            << "Depth: " << depth << std::endl
            << "Count: " << count << std::endl
            << "Points: " << dataset.points.size() << std::endl;

  auto total_time_rebuild = 0.0,
       total_time_index = 0.0;

  // Measure
  for (auto _ : state)
  {
    // Query
    std::cout << "Building " << count << " LSH-Tries with " << optimization_steps << " repetitions" << std::endl;

    auto start_rebuild = std::chrono::high_resolution_clock::now();
    auto maps = LSHMapFactory<D>::create_optimized(dataset.points, pool, depth, count, optimization_steps);
    auto end_rebuild = std::chrono::high_resolution_clock::now();
    auto time_rebuild = std::chrono::duration_cast<std::chrono::duration<double>>(end_rebuild - start_rebuild).count(); 
    total_time_rebuild += time_rebuild;
    
    auto start_index = std::chrono::high_resolution_clock::now();
    LSHForest<D> *index = new LSHForest<D>(maps, dataset.points, SingleBitFailure<D>);
    index->build();
    auto end_index = std::chrono::high_resolution_clock::now();
    auto time_index = std::chrono::duration_cast<std::chrono::duration<double>>(end_index - start_index).count(); 
    total_time_index += time_index;

    state.SetIterationTime(time_rebuild + time_index);
  }

  state.counters["input_size"] = DATA_SIZES_VALUE_LIST[sz]; 
  state.counters["optimization_steps"] = optimization_steps;
  state.counters["P1"] = P1;
  state.counters["P2"] = P1;
  state.counters["trie_depth"] = depth;
  state.counters["trie_count"] = count;
  state.counters["total_time_rebuild"] = total_time_rebuild;
  state.counters["total_time_index"] = total_time_index;
  state.counters["total_time"] = total_time_rebuild + total_time_index;
}

/**
 * @brief Benchmark the performance of building a LSH Forest index with a single thread, 
 *        and no trie rebuilding optimization
 * @param state { DataSize, OptimizationSteps, P1, P2 } 
 */
static void BM_build(benchmark::State &state) {
  srand(time(NULL));

  const DataSize sz = static_cast<DataSize>(state.range(0));  
  const float P1 = state.range(1) / 1000.0;
  const float P2 = state.range(2) / 1000.0;
  
  // Setup
  BenchmarkDataset<D> dataset = load_benchmark_dataset<D>(sz);
  HashFamily<D> pool = HashFamilyFactory<D>::createRandomBits(D);
  pool += HashFamilyFactory<D>::createRandomBits(D);
  pool += HashFamilyFactory<D>::createRandomBits(D);
  pool += HashFamilyFactory<D>::createRandomBits(D);

  const float depth_val = std::min(
    std::ceil(log(dataset.points.size()) / log(1 / P2)),
    30.0
  );

  const ui32 depth = depth_val;
  const ui32 count = std::ceil(std::pow(P1, -depth_val));


  std::cout << "<Started Benchmark of build>" << std::endl
            << "\tDepth: " << depth << std::endl
            << "\tCount: " << count << std::endl
            << "\tPoints: " << dataset.points.size() << std::endl;

  auto total_time_rebuild = 0.0,
       total_time_index = 0.0;

  // Measure
  for (auto _ : state)
  {
    // Query
    auto start_rebuild = std::chrono::high_resolution_clock::now();
    auto maps = LSHMapFactory<D>::create(pool, depth, count); // Generate maps without rebuilding optimization
    auto end_rebuild = std::chrono::high_resolution_clock::now();
    auto time_rebuild = std::chrono::duration_cast<std::chrono::duration<double>>(end_rebuild - start_rebuild).count(); 
    total_time_rebuild += time_rebuild;
    
    auto start_index = std::chrono::high_resolution_clock::now();
    LSHForest<D> *index = new LSHForest<D>(maps, dataset.points, SingleBitFailure<D>);
    index->build();
    auto end_index = std::chrono::high_resolution_clock::now();
    auto time_index = std::chrono::duration_cast<std::chrono::duration<double>>(end_index - start_index).count(); 
    total_time_index += time_index;

    state.SetIterationTime(time_rebuild + time_index);
  }

  state.counters["input_size"] = DATA_SIZES_VALUE_LIST[sz]; 
  state.counters["P1"] = P1;
  state.counters["P2"] = P1;
  state.counters["trie_depth"] = depth;
  state.counters["trie_count"] = count;
  state.counters["total_time_rebuild"] = total_time_rebuild;
  state.counters["total_time_index"] = total_time_index;
  state.counters["total_time"] = total_time_rebuild + total_time_index;
  std::cout << "<Finished Benchmark of build>\n" << std::endl;
}


// Add to benchmarks
BENCHMARK(BM_rebuild_mthread)
  ->Name("Build LSH Forest (Multi-Threaded)")
  ->Unit(benchmark::kMillisecond)
  ->Args({0, 20, 860, 535}) // XS
  ->Args({1, 20, 860, 535}) // S
  ->Repetitions(5)
  ->UseManualTime(); 

BENCHMARK(BM_rebuild)
  ->Name("Build LSH Forest (Single-Threaded)")
  ->Unit(benchmark::kMillisecond)
  ->Args({0, 20, 860, 535}) // XS
  ->Args({1, 20, 860, 535}) // S
  ->Repetitions(5)
  ->UseManualTime();

BENCHMARK(BM_build)
  ->Name("Build LSH Forest (Single-Threaded, No Rebuild)")
  ->Unit(benchmark::kMillisecond)
  ->Args({0, 860, 535}) // XS
  ->Args({1, 860, 535}) // S
  ->Repetitions(5)
  ->UseManualTime();