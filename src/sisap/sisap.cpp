#include <iostream>
#include <ctime>
#include <random>

#include "../global.hpp"
#include "../hash/hashpool.hpp"
#include "../hash/hashfamilyfactory.hpp"
#include "../index/lshmapfactory.hpp"
#include "../index/lshforest.hpp"
#include "../statistics/statsgenerator.hpp"
#include "../util/ranges.hpp"
#include "../hash/hashfamilyfactory.hpp"

#include "./io.hpp"

int main()
{
  // Parameters

  DataSize dataset_size = DataSize::XL;

  const float recall = 0.91f;
  
  const ui32 nrToQuery = 10; 

  const float P1 = 0.860f;
  const float P2 = 0.535f;

  const ui32 optimization_steps = 20;

  // Run
  srand(time(NULL));

  std::cout << "Loading benchmark dataset" << std::endl;
  PointsDataset<D> dataset = load_sisap<D>();
  PointsDataset<D> queries = load_sisap_queries<D>();

  std::cout << "Instantiating hash LSHMap" << std::endl;
  HashFamily<D> pool = HashFamilyFactory<D>::createRandomBits(D);

  const float depth_val = std::min(
    std::ceil(log(dataset.size()) / log(1 / P2)),
    30.0
  );

  const ui32 depth = depth_val;
  const ui32 count = std::ceil(std::pow(P1, -depth_val));

  std::cout << "Depth: " << depth << std::endl
            << "Count: " << count << std::endl
            << "Points: " << dataset.size() << std::endl;

  std::cout << "Instantiating maps" << std::endl;
  auto maps = LSHMapFactory<D>::mthread_create_optimized(dataset, pool, depth, count, optimization_steps);

  std::cout << "Building index" << std::endl;
  
  auto start_build = std::chrono::high_resolution_clock::now();

  LSHForest<D> *index = new LSHForest<D>(maps, dataset, SingleBitFailure<D>);
  index->build();

  auto end_build = std::chrono::high_resolution_clock::now();

  double total_build_time = std::chrono::duration_cast<std::chrono::duration<double>>(end_build - start_build).count();

  std::cout << "Running query" << std::endl;

  std::vector<std::vector<std::pair<ui32, ui32>>> results(queries.size());

  double total_time = 0;



  std::cout << "Answering queries with multiple threads..." << std::endl;
  // Query
  auto start = std::chrono::high_resolution_clock::now();

  std::vector<std::vector<ui32>> query_result = index->mthread_queries(queries, nrToQuery, recall);

  auto end = std::chrono::high_resolution_clock::now();
  total_time = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
  for (int i = 0; i < results.size(); ++i) {
    // Vector of pairs of { point index, distance to query point } 
    std::transform(ALL(query_result[i]), results[i].begin(), [&index, &queries, &i](ui32 j)
          { return std::pair<ui32,ui32>(j+1, queries[i].distance((*index)[j])); });
  }
  // Save results
  std::cout << "Saving results" << std::endl;

  std::stringstream ss;

  ss << "P1=" << P1 << ", P2=" << P2 << ", depth=" << depth << ", tries=" << count << ", optimization_steps=" << optimization_steps;

  save_results_to_hdf5("results_test.h5", results, dataset_size, total_build_time, total_time, ss.str());

  return 0;
}
