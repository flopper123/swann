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
