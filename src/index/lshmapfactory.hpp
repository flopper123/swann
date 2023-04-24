#pragma once

#include "../hash/hashfamily.hpp"
#include "lsharraymap.hpp"
#include "../statistics/lshmapanalyzer.hpp"

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
    // Vars for optimization
    auto OPTIMIZATION_ITERATIONS = 100;//D/2;
    auto OPTIMIZATION_THRESHOLD  = points.size() / (6 * depth);

    std::vector<LSHMap<D> *> ret;

    for (ui32 m = 0; m < k; ++m)
    {
      LSHMap<D> *hi = NULL;
      ui32 hi_count = UINT32_MAX;


      for (int i = 0; i < OPTIMIZATION_ITERATIONS; i++)
      {
        // Create new map
        LSHMap<D> *map = LSHMapFactory<D>::create(H, depth);

        // Fill with points
        map->add(points);
        
        // Map anaylzation
        LSHMapAnalyzer<D> analyzer(map);

        auto distribution = analyzer.getBucketDistribution();

        // Check if new best bucket has been found
        if (distribution.front() < hi_count)
        {
          std::cout << std::endl
                    << "New best bucket found for " << m << " at step " << i << std::endl
                    << "Threshold factor: " << (float)OPTIMIZATION_THRESHOLD / distribution.front() << std::endl
                    << std::endl
                    << std::endl;
          hi_count = distribution.front();
          hi = map;
        } else {
          delete map;
        }

        // Exit early if threshold is met
        if (hi_count < OPTIMIZATION_THRESHOLD)
        {
          std::cout << "Threshold met for " << m << " at step " << i << std::endl;
          break;
        }
      }

      ret.emplace_back(hi);
    }

    return ret;
  }
};
