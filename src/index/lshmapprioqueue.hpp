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
      return std::max_element(ALL(lhs->get_bucket_sizes())) < std::max_element(ALL(rhs->get_bucket_sizes()));
    }
};

// Thread safe priority queue for LSHMaps, ordering them by the size of their largest bucket
template<ui32 D>
class LSHMapPriorityQueue {
  // Underlying queue
  std::priority_queue<LSHMap<D>*, std::vector<LSHMap<D>*>, LSHMapBucketSizeCompare<D>> pqueue;

  // mutex for thread synchronization
  std::mutex mtx;

  // Condition variable for signaling
  std::condition_variable cond;

public:
  bool empty() const { return pqueue.empty(); }
  
  LSHMap<D>* top() const { return pqueue.top(); }
  
  void push(LSHMap<D>* lshmap) {
    std::unique_lock<std::mutex> lock(mtx);
    pqueue.push(lshmap);
    cond.notify_one();
  }
  
  void pop() {
    std::unique_lock<std::mutex> lock(mtx);
    
    // await queue to not be empty
    cond.wait(lock, [this]() { return !pqueue.empty(); }); 
    
    pqueue.pop();
  }
};