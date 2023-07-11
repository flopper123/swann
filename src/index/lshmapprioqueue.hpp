#pragma once

#include "../hash/hashfamily.hpp"
#include "lsharraymap.hpp"
#include "lshhashmap.hpp"
#include "../statistics/lshmapanalyzer.hpp"
#include "../util/ranges.hpp"

#include <mutex>
#include <condition_variable>

static ui32 maxBucketSize(LSHMap<D>* mp) {
  auto sizes = mp->get_bucket_sizes();
  return sizes.empty() ? 0 : *std::max_element(ALL(sizes));
}

template<ui32 D>
class LSHMapBucketSizeCompare {
  public:
    bool operator()(LSHMap<D>* lhs, LSHMap<D>* rhs) const {
      return maxBucketSize(lhs) < maxBucketSize(rhs);
    }
};

// Thread safe priority queue for LSHMaps, ordering them by the size of their largest bucket
template<ui32 D>
class LSHMapPriorityQueue {
  // Underlying queue
  std::priority_queue<LSHMap<D>*, std::vector<LSHMap<D>*>, LSHMapBucketSizeCompare<D>> pqueue;

  // mutex for thread synchronization
  std::mutex mtx;
  
  // Maximum number of elements in the queue
  ui32 max_size;

public:
  LSHMapPriorityQueue(ui32 max_size = 10) : max_size(max_size) {}

  bool empty() const { return pqueue.empty(); }

  /**
   * @brief Get all elements in the queue, emptying it in the process
  */
  std::vector<LSHMap<D>*> get_all() {
    std::unique_lock<std::mutex> lock(mtx);
    std::vector<LSHMap<D>*> ret;
    while (!pqueue.empty()) {
      ret.emplace_back(pqueue.top());
      pqueue.pop();
    }
    return ret;
  }

  // top of queue always points the map with most points in it's largest bucket
  LSHMap<D>* top() const { return pqueue.top(); }
  
  bool push(LSHMap<D>* mp) {
    // Acquire an unique lock for the queue
    std::unique_lock<std::mutex> lock(mtx);

    // If the queue is not full, push the map
    if (pqueue.size() < this->max_size) {
      pqueue.push(mp);
      return true;
    }

    // If the queue is full, check if the top element has a larger bucket than the map to be pushed
    bool hasPushed = false;
    const ui32 topSize = maxBucketSize(pqueue.top()),
               mpSize = maxBucketSize(mp);

    if (topSize > mpSize)
    {
      pqueue.pop();
      pqueue.push(mp);
      hasPushed = true;
    }

    return hasPushed;
  }
};