#pragma once

#include "../hash/hashfamily.hpp"
#include "lsharraymap.hpp"
#include "lshhashmap.hpp"
#include "lshmapprioqueue.hpp"
#include "../statistics/lshmapanalyzer.hpp"
#include "../util/ranges.hpp"
#include <thread>

template<ui32 D> 
class LSHMapFactory {
private: 
  LSHMapFactory() {}

public:
  static LSHMap<D>* create(HashFamily<D>& H, ui32 depth) {
    auto hf = H.subset(depth);
    return new LSHHashMap<D>(hf);
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
        new LSHHashMap<D>(hf)
      );
    }

    return ret;
  }

  static std::vector<LSHMap<D> *> create_optimized(std::vector<Point<D>> &points, HashFamily<D> &H, ui32 depth, ui32 k, ui32 steps = 1) {
    std::vector<LSHMap<D> *> ret;

    ui32 OPTIMIZATION_ITERATIONS = steps;

    std::cout << "Optimizing buckets with " << OPTIMIZATION_ITERATIONS << " iterations" << std::endl << std::endl;

    ui32 largest_bucket = 0;
    double largest_dev = 0.0;

    for (ui32 m = 0; m < k; ++m)
    {
      LSHMap<D> *hi = LSHMapFactory<D>::create(H, depth),  // points are never inserted into hi
                *map = LSHMapFactory<D>::create(H, depth); // tmp map used to find the best hash family

      ui32 hi_count = UINT32_MAX;
      double hi_dev = 0.0;

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
          hi_dev = Util::norm_std_dev(ALL(distribution));

          hi->build(hsubset); // Rebuild hi with the new hash family
        }
      }

      if (largest_bucket < hi_count) {
        largest_bucket = hi_count;
      }

      if (largest_dev < hi_dev) {
        largest_dev = hi_dev;
      }

      delete (map); // Delete the map we used for optimization to avoid memleak
      ret.emplace_back(hi);
    }

    std::cout << "Largest bucket in all tries: " << largest_bucket << std::endl;
    std::cout << "Largest dev in all tries: " << largest_dev << std::endl << std::endl;

    return ret;
  }
  /**
   * @brief Construct @k LSHMaps with @depth hashfunctions chosen from @H
   *        The hash functions are chosen to minimize the size of the largest bucket
   *        and the building process is handled by multiple threads.
   * @param points The input points to build the LSHMaps from
   * @param H The hash family to choose hash functions from
   * @param depth The number of hash functions per LSHMap
   * @param steps The number of times to rebuild each LSHMap, the best ones out of @k*@steps builds are chosen.
   *              It defaults to 1, which means that the LSHMaps are not rebuild.
  */
  static std::vector<LSHMap<D> *> mthread_create_optimized(
    std::vector<Point<D>> &points, 
    HashFamily<D> &H, 
    ui32 depth, 
    ui32 k, 
    ui32 steps = 1) 
  {
    // Make sure to initialize the masks before starting threads
    LSHHashMap<D>::initMasks(depth);
    
    const ui32 THREAD_CNT = std::thread::hardware_concurrency();
    const ui32 THREAD_STEPS = std::ceil(k * steps / ((double) THREAD_CNT));
    assert(THREAD_CNT * steps >= k); 
    
    LSHMapPriorityQueue<D> mqueue(H, k, depth);

    // Each thread builds @THREAD_STEPS LSHMaps from @points
    // and try to insert them into the priority queue
    auto build_map = [&mqueue, &points, &depth, &H, &THREAD_STEPS](int id)
    {
      LSHMap<D> *map = LSHMapFactory<D>::create(H, depth); // Temporary map to find good hash families
      for (ui32 i = 0; i < THREAD_STEPS; i++)
      {
        HashFamily<D> hsubset = H.subset(depth);
        map->build(hsubset); // Clears the map and builds it with the new hash family
        map->add(points);
        mqueue.push(map); // Attempt to push the map into the priority queue

        //! comment line above and uncomment this to see it works:
        //! if (mqueue.push(map)) std::cout << "Thread " << id << " found a new map at step " << i << std::endl;
      }
      delete map;
    };
    
    // spawn threads that build_map
    std::vector<std::thread> pool(THREAD_CNT);
    for (int i = 0; i < THREAD_CNT; ++i) {
      pool[i] = std::thread(build_map, i);
    }
    
    // wait for threads to finish
    for (auto& th : pool) {
      th.join();
    }

    auto ret = mqueue.get_all();
    assert(ret.size() == k);
    return ret;
  }
};
