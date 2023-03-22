#pragma once

#include "../global.hpp"
#include "../index/point.hpp"

template<ui32 D>
class PointsDataset : public std::vector<Point<D>> {
  using std::vector<Point<D>>::vector;
};

// Response
template<ui32 D>
struct KNNResponse {
  Point<D> query;
  std::vector<ui32> nearest_neighbors;
};

template<ui32 D>
class QueryDataset : public std::vector<KNNResponse<D>> {
  using std::vector<KNNResponse<D>>::vector;
};


// Test datasets
template<ui32 D>
struct BenchmarkDataset {
  PointsDataset<D> points;
  QueryDataset<D> queries;
};

/**
 * @brief Calculates recall based on hamming distance of points
 */
inline static double calculateRecall(std::vector<ui32> actual_nn, std::vector<ui32> true_nn) {
  assert(actual_nn.size() <= true_nn.size());
  
  int count = 0;
  
  for (int i = 0; i < actual_nn.size(); i++) {
    if (actual_nn[count] == true_nn[i]) {
      count++;
    }
  }

  return (double)count / (double)actual_nn.size();
}