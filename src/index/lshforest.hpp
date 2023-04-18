#pragma once

#include <unordered_set>
#include "query/pointmap.hpp"
#include "index.hpp"
#include "lshmap.hpp"
#include "lshmapfactory.hpp"
#include "query/failureprob.hpp"

const QueryFailureProbability DEFAULT_FAILURE = TestSizeFailure;

template<ui32 D>
class LSHForest : public Index<D> {
   // A function that calculates the failure-probability of an ongoing query
  QueryFailureProbability is_exit; 
  
  // The minimum depth of all LSHMaps in the forest
  ui32 depth = UINT32_MAX;
  
  // The points in the forest
  std::vector<Point<D>> points;

  // The trees (LSHMaps) in the forest
  std::vector<LSHMap<D>*> maps;

public:
  LSHForest(
    std::vector<LSHMap<D>*>& maps, 
    QueryFailureProbability failure_strategy = DEFAULT_FAILURE
  ) : is_exit(failure_strategy) 
  {
    this->maps = maps;
    for (auto& map : maps) {
      this->depth = std::min(this->depth, map->depth());
    }
  };

  LSHForest(
    std::vector<LSHMap<D>*> &maps, 
    std::vector<Point<D>> &input, 
    QueryFailureProbability failure_strategy = DEFAULT_FAILURE
  ) : is_exit(failure_strategy) 
  {
    this->maps = maps;
    this->points = input;
    
    for (auto& map : maps) {
      this->depth = std::min(this->depth, map->depth());
    }
  };
  
  std::vector<LSHMap<D>*>& getMaps() { return maps; }

  ui32 size() { return points.size(); };
  
  // If we care about build performance, this needs to be emplace_back, and then we should implement a copy constructor for points
  void insert(Point<D>& point) { points.push_back(point); }; 
  
  void build() {
    for (auto &map : this->maps) {
      map->add(this->points);
    }
  };

  std::vector<ui32> query(const Point<D>& point, int k, float recall = 0.8) 
  {
    const ui32 M = this->maps.size();
    PointMap<D> found(&(this->points), point);
    std::vector<ui32> hash(M);  // hash[m] : contains the hash of point in map[m]

    for (ui32 m = 0; m < M; ++m){
      hash[m] = this->maps[m]->hash(point);
    }

    // Loop through all buckets within hamming distance of hdist of point
    for (ui32 hdist = 0;
         hdist < this->depth && !stop_query(recall, hdist, found.size(), k, found.get_kth_dist(k));
         ++hdist)
    {
      // Loop through all maps
      for (ui32 m = 0; m < M; ++m)
      {   
        // Loop through all buckets with hamming distance of hdist to point
        for (ui32 bucket_i : maps[m]->query(hash[m], hdist)) {
          found.insert(ALL((*maps[m])[bucket_i]));
        }
      }
    }
      
    return found.get_k_nearest(k);
  }

  Point<D> &operator[](ui32 i) { return points[i]; };

private:
  /**
   * @brief Returns true if the kNN-query should stop
   * @param recall A decimal value between 0 and 1 indicating 
   *               the number of correct points to find
   * @param depth Current depth of the tree being queried
   * @param found Number of points found so far
   * @param tar Number of kNN to find
  */
  bool stop_query(float recall, ui32 curDepth, ui32 found, ui32 tar, ui32 kthHammingDist) const
  {
    const float failure_prob = is_exit(this->maps.size(), this->depth, curDepth, found, tar, kthHammingDist);
    return (1.0 - recall) >= failure_prob && found >= tar;
  }
};
