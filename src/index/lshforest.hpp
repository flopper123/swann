#pragma once

#include "index.hpp"
#include "lshmap.hpp"
#include "lshmapfactory.hpp"

template<ui32 D>
class LSHForest : Index<D> {
  std::vector<Point<D>> points;
  std::vector<LSHMap<D>> maps;

  /** Calculate number of maps to initialize based on points*/
  ui64 nr_maps() {
    return max(this->points.size() / 100, 5);
  }

public:
  LSHForest(std::vector<LSHMap<D>>& maps) {
    this->maps = maps;
  };

  LSHForest(std::vector<LSHMap<D>> &maps, std::vector<Point<D>> &input)
  {
    this->maps = maps;
    this->points = input;
  };

  ui32 size() { return points.size(); };
  
  void insert(Point<D>& point) {
    points.push_back(point);
  };
  
  void build() {
    // Insert points into all LSHMaps
    for(auto& map : maps) {
      map.add(this->points);
    }
  };  

  std::vector<ui32> query(const Point<D>& point, int k) {
    std::set<ui32> ret;

    for (const LSHMap<D>& map : maps) {
      hash_idx idx = map.hash(point);

      // for (int i = 0; ret.size() < k; i++) {
      //   for (auto bucket : map.query(idx, i)) {
      //     ret.insert(ALL(map[bucket]));
      //   }
      // }
      
      std::vector<hash_idx> buckets = map.query(idx);

      for (hash_idx bucket : buckets) {
        ret.insert(ALL(map[bucket]));
      }
    }
    
    assert(ret.size() >= k);

    return std::vector<ui32>(ALL(ret));
  }

  Point<D> &operator[](ui32 i) { return points[i]; };
};


