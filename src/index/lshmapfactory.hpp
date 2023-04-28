#pragma once

#include "../hash/hashfamily.hpp"
#include "lsharraymap.hpp"
#include "../statistics/lshmapanalyzer.hpp"
#include "../util/ranges.hpp"

template<ui32 D> 
class LSHMapFactory {
private: 
  LSHMapFactory() {}

public:
  static LSHMap<D>* create(HashFamily<D>& H, ui32 depth) {
    auto hf = H.subset(depth);
    return new LSHArrayMap<D>(hf);
  }

  /** 
   * @brief Construct a @k LSHMaps with @depth hashfunctions chosen @H 
   * @param depth number of hash functions per map 
   * @param k number LSHMaps to construct
   * @returns a vector of LSHMaps constructed from random hash functions of @H
   **/
  static std::vector<LSHMap<D>*> create(HashFamily<D>& H, ui32 depth, ui32 k) {
    // Get unique number of hash functions
    HashFamily<D> subset = H.subset(k * depth);
    
    std::vector<LSHMap<D>*> ret;
    
    for (ui32 i = 0; i < k; ++i) {
      auto hf = HashFamily<D>(
        subset.begin() + i * depth,
        subset.begin() + (i + 1) * depth
      );

      ret.push_back(
        new LSHArrayMap<D>(hf)
      );
    }

    return ret;
  }

  static std::vector<LSHMap<D> *> create_optimized(std::vector<Point<D>> &points, HashFamily<D> &H, ui32 depth, ui32 k) {
    std::vector<LSHMap<D> *> ret;

    // Vars for optimization
    ui32 OPTIMIZATION_ITERATIONS = 1000;
    ui32 LARGEST_BUCKET_THRESHOLD = 5000;

    assert(2 * (points.size() / std::pow(2, depth)) < LARGEST_BUCKET_THRESHOLD);


    ui32 largest_bucket = 0;

    for (ui32 m = 0; m < k; ++m)
    {
      LSHMap<D> *hi = LSHMapFactory<D>::create(H, depth),  // points are never inserted into hi
                *map = LSHMapFactory<D>::create(H, depth); // tmp map used to find the best hash family

      ui32 hi_count = UINT32_MAX;

      bool threshold_met = false;

      for (ui32 i = 0; i < OPTIMIZATION_ITERATIONS; i++)
      {
        HashFamily<D> hsubset = H.subset(depth);
        map->build(hsubset);
        map->add(points);
        
        // Map anaylzation, one line it to make it explicit that we dont need to remember the ptr to map
        std::vector<ui32> distribution = LSHMapAnalyzer<D>(map).getBucketDistribution();

        // Check if new best bucket has been found
        if (distribution.front() <= hi_count)
        {
          hi_count = distribution.front();
          hi->build(hsubset); // Rebuild hi with the new hash family
        }
        
        // Exit early if threshold is met to avoid overfitting (i.e. using the same hash family for all maps)
        if (hi_count <= LARGEST_BUCKET_THRESHOLD)
        {
          std::cout << "Threshold met for " << m << " at step " << i << std::endl << std::endl;
          threshold_met = true;
          break;
        }
      }

      if (largest_bucket < hi_count) {
        largest_bucket = hi_count;
      }

      if (!threshold_met) {
        std::cout << "Threshold not met for " << m << " with count " << hi_count << std::endl << std::endl;
      }

      delete (map); // Delete the map we used for optimization to avoid memleak
      ret.emplace_back(hi);
    }

    std::cout << "Largest bucket: " << largest_bucket << std::endl << std::endl;

    return ret;
  }
};
