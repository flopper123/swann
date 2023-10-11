#pragma once

#include <unordered_set>

#include "./query/pointmap.hpp"
#include "index.hpp"
#include "lshmap.hpp"
#include "lshmapfactory.hpp"
#include "./query/failureprob.hpp"

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
  std::vector<LSHMap<D>*>& maps;

public:
  LSHForest(std::vector<LSHMap<D>*> &maps, 
            std::vector<Point<D>> &input, 
            QueryFailureProbability failure_strategy = DEFAULT_FAILURE) 
    : is_exit(failure_strategy), 
      depth(maps.empty() ? 0 : maps.front()->depth()), 
      points(input), 
      maps(maps)
  {};

  ~LSHForest() {
    this->points.clear();
    
    while(!this->maps.empty())
    {
      LSHMap<D>* m = this->maps.back();
      this->maps.pop_back();
      delete m;
    }
  }
  
  const std::vector<LSHMap<D>*>& getMaps() { return maps; }

  ui32 size() const noexcept { return points.size(); };
  
  // If we care about build performance, this needs to be emplace_back, and then we should implement a copy constructor for points
  void insert(Point<D>& point) { points.push_back(point); }; 
  
  void build() {
    for (auto &map : this->maps) {
      if (map->size() < this->size()) {
        map->add(this->points);
      }
    }
  };
  
  inline float get_bucket_factor(const float recall) const noexcept {
    const float recall_factor = (1.0 - recall) / 0.05;
    const float bucket_frac = ((1000.0 + std::log2(this->maps.front()->bucketCount())) * this->maps.size());
    const float val = (this->size() * (std::pow(recall,recall_factor-1))) / bucket_frac + 40.0;
    return std::numbers::e * val;
  }
  
  /**
   * @brief Returns the indices of the @k nearest neighbors to for a list of points, where
   *        atleast a @recall fraction of the points are among the true kNN on avg. 
   *        The queries are handled by @thread_cnt threads.
   * @param queries Points to query for
   * @param k Number of nearest neighbors to find for each point
   * @param recall The precision of the query
   * @param thread_cnt The number of threads to use, defaults to std::thread::hardware_concurrency().
   * @return std::vector<std::vector<ui32>> For which the i'th vector contains the indices of the k-nearest-neighbours 
   *         in ascending order by distance for @queries[i]. 
   */
  std::vector<std::vector<ui32>> mthread_queries(std::vector<Point<D>>& queries, int k, float recall = 0.9, ui32 thread_cnt = 0) {
    assert(thread_cnt <= std::thread::hardware_concurrency());
    if (!thread_cnt) thread_cnt = std::thread::hardware_concurrency();

    std::vector<std::vector<ui32>> results(queries.size(), std::vector<ui32>()),
                                   batches(thread_cnt, std::vector<ui32>()); // indices of points in queries
    
    // Split the queries into thread_cnt batches
    for (ui32 i = 0; i < queries.size(); ++i) {
      batches[i % thread_cnt].emplace_back(i);
    }

    auto answer_batch = [this, k, recall, &queries, &results, &batches](ui32 bidx) {
      for (auto& pidx : batches[bidx]) {
        results[pidx] = this->query(queries[pidx], k, recall);
      }
    };
    
    // Spawn threads that answer queries
    std::vector<std::thread> pool(thread_cnt);
    for (ui32 i=0; i < thread_cnt; ++i) {
      pool[i] = std::thread(answer_batch, i);
    }
      
    // wait for threads to finish
    for (auto& th : pool) {
      th.join();
    }
  
    return results;
  }

  /**
   * @brief Returns the indices of the @k nearest neighbors to @point where
   *        atleast a @recall fraction of the points are among the true kNN.
   * @param point Point to query for
   * @param k Number of nearest neighbors to find
   * @param recall The precision of the query
   * @param log A ptr to a query log to use for storing additional query information.
   * @return std::vector<ui32> A vector of size @k containing the indices of the k-nearest-neighbours 
   *         in ascending order by distance. 
   */
  std::vector<ui32> query(const Point<D>& point, int k, float recall = 0.9, QueryLog *log = nullptr)
  {
    PointMap<D> found(this->points, point, k);  // found : contains the k nearest points found so far and look up of seen points

    const ui32 M = this->maps.size(), 
               BATCH_SIZE = k * this->get_bucket_factor(recall);

    std::vector<ui32> hash(M); // hash[m] : contains the hash of point in map[m]
    for (ui32 m = 0; m < M; ++m){
      hash[m] = this->maps[m]->hash(point);
    }
    
    // Loop through all buckets within hamming distance of hdist of point
    ui32 hdist = 0, mask_index = 0, buckets = 0;
    while (hdist < this->depth) 
    {
      for (ui32 m = 0; m < M; ++m)
      {
        ui32 bucket_index = maps[m]->next_bucket(hash[m], hdist, mask_index);
        found.insert(ALL((*maps[m])[bucket_index]));
        ++buckets;
        if (found.size() >= k)
        {
          float fail_prob = SingleBitFailure_V2<D>(M, m + 1, this->depth, hdist, found.get_kth_dist(), mask_index+1);
          if (fail_prob <= (1.0 - recall)) {
            if (log) {
              log->mask_index = mask_index;
              log->hdist = hdist;
              log->found = found.size();
              log->visited = buckets;
              log->fail_prob = fail_prob;
            }
            return found.extract_k_nearest();
          }
        }
      }
      
      // If one map has next bucket they all do, so we just check for an arbitrary map
      if (!this->maps[0]->has_next_bucket(hash[0], hdist, ++mask_index)) {
        ++hdist;
        mask_index = 0;
      }
    }

    if (log) {
      log->mask_index = mask_index;
      log->hdist = hdist;
      log->found = found.size();
      log->visited = buckets;
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
      
    // http://madscience.ucsd.edu/2020/notes/lec13.pdf
    const bool earlyFinish = 2000 * this->maps.size() < found;

    const float failure_prob = is_exit(this->maps.size(), this->depth, curDepth, found, tar, kthHammingDist);
    return earlyFinish || (failure_prob <= (1.0 - recall) && found >= tar) || (curDepth >= 8 && found >= tar);
  }
};
