#pragma once

#include "../hash/hashfamily.hpp"
#include "lsharraymap.hpp"
#include "lshhashmap.hpp"
#include "../statistics/lshmapanalyzer.hpp"
#include "../util/ranges.hpp"

#include <mutex>
#include <condition_variable>

template<ui32 D>
class LSHMapBucketSizeCompare {
  public:
    bool operator()(LSHMap<D>* lhs, LSHMap<D>* rhs) const {
      return lhs->maxBucketSize() > rhs->maxBucketSize();
    }
};

/**
 * @brief Thread safe priority queue for LSHMaps, ordering them by the size of their largest bucket.
 *        Internally stores the maps in a Vector sorted by LSHMapBucketSize comparison such that front 
 *        is map with largest bucket. An advantage of using a vector instead of the std::priority_queue,
 *        is that get_all is non-destructive.
 *        To avoid memory leaks the queue never copies the points from the maps that are pushed to the queue,
 *        but only shallow copies the max_bucket_size and hashes.
 */
template<ui32 D>
class LSHMapPriorityQueue {
  std::vector<LSHMap<D>*> pqueue;

  // mutex for thread synchronization
  std::mutex mtx;
  
  // Maximum number of elements in the queue
  ui32 max_size, sz;

public:
  LSHMapPriorityQueue(HashFamily<D>& hf, ui32 max_size = 10, ui32 depth = 5) : max_size(max_size), sz(0) {
    // Initialize queue with max_size maps
    for (ui32 i = 0; i < max_size; ++i) {
      HashFamily<D> hashes = hf.subset(depth); 
      LSHMap<D>* mp = new LSHHashMap<D>(hashes);
      mp->max_bucket_size = UINT32_MAX;
      pqueue.push_back(mp);
    }
  }
  ui32 capacity() const { return max_size; }
  ui32 size() const { return sz; }

  // top of queue always points to the map with most points in it's largest bucket
  LSHMap<D>* top() const { return pqueue.front(); }

  bool empty() const { return sz == 0; }

  /**
   * @brief Get all elements in the queue
  */
  std::vector<LSHMap<D>*> get_all() {
    std::unique_lock<std::mutex> lock(mtx);
    return pqueue;
  }
  
  bool push(LSHMap<D>* mp) {
    // Acquire an unique lock for the queue
    std::unique_lock<std::mutex> lock(mtx);

    // If the queue is full, check if the top element has a larger bucket than the map to be pushed
    bool hasPushed = false;
    if (pqueue[0]->maxBucketSize() > mp->maxBucketSize())
    {
      pqueue[0]->build(mp->hashes);
      pqueue[0]->max_bucket_size = mp->maxBucketSize();
      if (sz < max_size) ++sz;
      
      std::sort(ALL(pqueue), LSHMapBucketSizeCompare<D>());
      hasPushed = true;
    }

    return hasPushed;
  }
};