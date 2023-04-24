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
  const ui32 depth;
  
  // The points in the forest
  std::vector<Point<D>>& points;

  // The trees (LSHMaps) in the forest
  const std::vector<LSHMap<D>*>& maps;

public:
  LSHForest(std::vector<LSHMap<D>*> &maps, std::vector<Point<D>> &input, QueryFailureProbability failure_strategy = DEFAULT_FAILURE) 
    : is_exit(failure_strategy), 
      depth(maps.empty() ? 0 : maps.front()->depth()), 
      points(input), 
      maps(maps)
  {};
  
  const std::vector<LSHMap<D>*>& getMaps() { return maps; }

  ui32 size() const noexcept { return points.size(); };
  
  // If we care about build performance, this needs to be emplace_back, and then we should implement a copy constructor for points
  void insert(Point<D>& point) { points.push_back(point); }; 
  
  void build() {
    for (auto &map : this->maps) {
      map->add(this->points);
    }
  };
  
  /**
   * @brief Returns the indices of the @k nearest neighbors to @point where
   *        atleast a @recall fraction of the points are among the true kNN.
   * @param point Point to query for
   * @param k Number of nearest neighbors to find
   * @param recall The precision of the query
   * @return std::vector<ui32> A vector of size @k containing the indices of the k-nearest-neighbours 
   *         in ascending order by distance. 
   */
  std::vector<ui32> query(const Point<D>& point, int k, float recall = 0.8) const noexcept
  {
    PointMap<D> found(this->points, point, k);  // found : contains the k nearest points found so far and look up of seen points
    
    const ui32 M = this->maps.size();
    std::vector<ui32> hash(M);                  // hash[m] : contains the hash of point in map[m]
    for (ui32 m = 0; m < M; ++m){
      hash[m] = this->maps[m]->hash(point);
    }

    // Loop through all buckets within hamming distance of hdist of point
    ui32 hdist = 0, mask_index = 0;
    while (hdist < this->depth && !stop_query(recall, hdist, found.size(), k, found.get_kth_dist())) {
      for (ui32 m = 0; m < M; ++m) {
        ui32 bucket_index = maps[m]->next_bucket(hash[m], hdist, mask_index);
        found.insert(ALL((*maps[m])[bucket_index]));
      }
      
      mask_index++;
      // If one map has next bucket they all do, so we just check for an arbitrary map
      if (!this->maps[0]->has_next_bucket(hash[0], hdist, mask_index)) {
        ++hdist;
        mask_index = 0;
      }
    }
    
    return found.extract_k_nearest();
  }

  const Point<D> &operator[](ui32 i) const noexcept { return points[i]; };

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
    return failure_prob <= (1.0 - recall) && found >= tar;
  }
};
