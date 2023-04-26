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
    ui32 OPTIMIZATION_ITERATIONS = 100;
    ui32 LARGEST_BUCKET_THRESHOLD = 4500;
    ui32 LARGEST_LAYER_THRESHOLD = LARGEST_BUCKET_THRESHOLD * k;

    assert(points.size() / std::pow(2, depth) < LARGEST_BUCKET_THRESHOLD);


    ui32 largest_bucket = 0;

    for (ui32 m = 0; m < k; ++m)
    {
      LSHMap<D> *hi = LSHMapFactory<D>::create(H, depth),  // points are never inserted into hi
                *map = LSHMapFactory<D>::create(H, depth); // tmp map used to find the best hash family

      ui32 hi_count = UINT32_MAX;

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
          std::cout << "New best bucket found for " << m << " at step " << i << "\n"
                    << "Threshold factor: " << (float)LARGEST_BUCKET_THRESHOLD / distribution.front() << "\n"
                    << std::endl;
          hi_count = distribution.front();
          hi->build(hsubset); // Rebuild hi with the new hash family
        }
        
        // Exit early if threshold is met to avoid overfitting (i.e. using the same hash family for all maps)
        if (hi_count <= LARGEST_BUCKET_THRESHOLD)
        {
          std::cout << "Threshold met for " << m << " at step " << i << std::endl << std::endl;
          break;
        }
      }

      if (largest_bucket < hi_count) {
        largest_bucket = hi_count;
      }

      delete (map); // Delete the map we used for optimization to avoid memleak
      ret.emplace_back(hi);
    }

    std::cout << "LARGEST BUCKET: " << largest_bucket << std::endl << std::endl;

    return ret;
  }
};
