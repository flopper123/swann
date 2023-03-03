#pragma once

#include "index.hpp"
#include "lshmap.hpp"
#include "lshmapfactory.hpp"
#include "query/failureprob.hpp"

const QueryFailureProbability DEFAULT_FAILURE = SimpleSizeFailure;

template<ui32 D>
class LSHForest : public Index<D> {
   // A function that calculates the failure-probability of an ongoing query
  QueryFailureProbability is_exit; 
  
  // The minimum depth of all LSHMaps in the forest
  ui32 depth;
  
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
  
  void insert(Point<D>& point) { points.push_back(point); };
  
  void build() {
    // Insert points into all LSHMaps
    for(auto& map : maps) {
      map->add(this->points);
    }
  };  

  std::vector<ui32> query(const Point<D>& point, int k, float recall = 0.8) 
  {
    std::set<ui32> found;                                // A set containing all found points so far from the maps
    std::vector<ui32> hash_idx(this->maps.size()), ret;  // hash[m] : contains the hash of point in map[m]
    std::vector<std::pair<ui32, ui32>> pdist;            // A vector containing pairs of (dist, idx)
    const ui32 M = this->maps.size();
  
    for (ui32 m = 0; m < M; ++m) {
      hash_idx[m] = maps[m]->hash(point);
    }
    
    // Loop through all buckets within hamming distance of hdist of point
    for (ui32 hdist = 0;
         hdist < this->depth && !stop_query(recall, hdist, found.size(), k);
         ++hdist)
    {
      // Loop through all maps
      for (ui32 m = 0; m < M; ++m)
      {
        // Loop through all buckets with hamming distance of hdist to point
        for (ui32 bucket_i : maps[m]->query(hash_idx[m], hdist))
        {
          found.insert(ALL((*maps[m])[bucket_i]));
        }
      }
    }
    
    // Transform all found points into pairs of (distance, point idx)
    std::transform(ALL(found), std::back_inserter(pdist), [this, point](ui32 pi) -> std::pair<ui32,ui32> { 
      return { point.distance(this->points[pi]), pi}; });

    // Sort all found points in ascending order by distance to point
    std::sort(ALL(pdist), std::less<std::pair<ui32,ui32>>()); 
    
    // Copy the first k points into the return vector
    std::transform(pdist.begin(), pdist.begin() + k, 
                   std::back_inserter(ret), 
                   [](auto &p){ return p.second; });
    return ret;
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
  bool stop_query(float recall, ui32 curDepth, ui32 found, ui32 tar) const
  {
    const float failure_prob = is_exit(this->maps.size(), this->depth, curDepth, found, tar);
    return (1 - recall) > failure_prob;
  }

};


