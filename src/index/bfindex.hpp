#pragma once

#include "index.hpp"

template<int D>
class BFIndex : Index<D> {
  std::vector<Point<D>> points;

  public:
    BFIndex() {
      std::cout << "I am building\n";
    }

    BFIndex(std::vector<Point<D>>& points) {
      std::cout << "I am building\n";
      this->points = points;
    }
    
    ui32 size() { 
      return this->points.size(); 
    }
    
    void build() {}

    void insert(Point<D>& p) {
      this->points.push_back(p);
    }

    std::vector<ui32> query(const Point<D>& point, int k) {
      // Create distance vector
      std::vector<std::pair<ui32, ui32>> distance;

      // Find Hamming distance to all points
      for (ui32 i = 0; i < this->size(); i++) {
        distance.emplace_back(Point<D>.distance(point, this->points[i]), i);
      }
      
      // Sort list
      std::sort(ALL(distance));

      // Take k elements with lowest distance
      std::vector<ui32> ret;
      std::transform(distance.begin(), distance.begin() + k,
                     ret.begin(),
                     [](const pair<ui32, ui32> &dist) -> ui32
                     { return dist.second; });
      return ret;
    }

    Point<D>& operator[](ui32 i) {
      return points[i];
    }
};
