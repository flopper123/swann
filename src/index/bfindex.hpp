#pragma once

#include "index.hpp"

template<ui32 D>
class BFIndex : public Index<D> {
  std::vector<Point<D>>& points;

  public:
    BFIndex() 
    {}

    BFIndex(std::vector<Point<D>>& points) : points(points) 
    {}
    
    ui32 size() const noexcept { 
      return this->points.size(); 
    }
    
    void build(ui32 max_hdist = 0) {}

    void insert(Point<D>& p) {
      this->points.push_back(p);
    }

    std::vector<ui32> query(const Point<D>& point, int k, float recall = 0.8, QueryLog *log = nullptr) {
      // Create distance vector
      std::vector<std::pair<ui32, ui32>> distance;
      
      if (k > this->size())
        k = this->size();

      // Find Hamming distance to all points
      for (ui32 i = 0; i < this->size(); i++) {
        distance.emplace_back(this->points[i].distance(point), i);
      }
      
      // Sort list
      std::sort(ALL(distance));

      // Take k elements with lowest distance
      std::vector<ui32> ret(k, 0x0);
      std::transform(distance.begin(), distance.begin() + k,
                     ret.begin(),
                     [](const std::pair<ui32, ui32> &dist)
                     { return dist.second; }
      );
      return ret;
    }

    const Point<D>& operator[](ui32 i) const noexcept {
      return points[i];
    }
};
