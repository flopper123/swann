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
  ui32 stop_found;
  ui32 stop_hdist;
  ui32 stop_mask_index;
  ui32 buckets_visited;
  
  LSHForest(std::vector<LSHMap<D>*> &maps, std::vector<Point<D>> &input, QueryFailureProbability failure_strategy = DEFAULT_FAILURE) 
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
    const float val = (this->size() * (std::pow(recall,recall_factor-1))) / ((1000.0 + std::log2(this->maps.front()->bucketCount())) * this->maps.size()) + 40.0;
    return std::numbers::e * val;
  }

  /**
   * @brief Returns the indices of the @k nearest neighbors to @point where
   *        atleast a @recall fraction of the points are among the true kNN.
   * @param point Point to query for
   * @param k Number of nearest neighbors to find
   * @param recall The precision of the query
   * @return std::vector<ui32> A vector of size @k containing the indices of the k-nearest-neighbours 
   *         in ascending order by distance. 
   */
  std::vector<ui32> query(const Point<D>& point, int k, float recall = 0.9)
  {
    // std::cout << "Querying for point: " << point << " with k = " << k << " and recall = " << recall << std::endl;
    PointMap<D> found(this->points, point, k);  // found : contains the k nearest points found so far and look up of seen points
     
    const ui32 M = this->maps.size(), BATCH_SIZE = k * this->get_bucket_factor(recall);

    std::vector<ui32> hash(M); // hash[m] : contains the hash of point in map[m]
    for (ui32 m = 0; m < M; ++m){
      hash[m] = this->maps[m]->hash(point);
    }
    
    // Loop through all buckets within hamming distance of hdist of point
    ui32 hdist = 0, mask_index = 0, buckets = 0;
    while (hdist < this->depth) 
    {
      ui32 hi = found.get_kth_dist();
      std::vector<ui32> bucket[M];
      std::queue<std::pair<ui32,ui32>> bucket_q; // bucket_q : contains the indices of the points in bucket[m] that are not in found
      for (ui32 m = 0; m < M; ++m)
      {
        ui32 bucket_index = maps[m]->next_bucket(hash[m], hdist, mask_index);
        bucket[m] = (*maps[m])[bucket_index];
        bucket_q.emplace(m, 0);
      }
        
      // To circumvent having to check the entire bucket, we start by checking the first BATCH_SIZE points in each bucket
      for (ui32 i = 0; !bucket_q.empty(); ++i)
      {
        auto [m, j] = bucket_q.front();
        bucket_q.pop();

        const ui32 end_idx = std::min(j + BATCH_SIZE, (ui32) bucket[m].size());

        // add points from bucket[m][j..j+BATCH_SIZE]
        found.insert(bucket[m].begin()+j, bucket[m].begin() + end_idx);
        
        // add the next batch from bucket to bucket_q if there is one
        if (end_idx < bucket[m].size()) 
        {
          bucket_q.emplace(m, end_idx);
        }

        // If we have a new kth distance and we have checked atleast M batches, then we check if we should stop
        if (i >= M && (hi != found.get_kth_dist()) && stop_query(recall, log2(buckets), found.size(), k, found.get_kth_dist()))
        {
          hi = found.get_kth_dist();
          this->stop_found = found.size();
          this->stop_hdist = hdist;
          this->stop_mask_index = mask_index;
          this->buckets_visited = buckets;
          return found.extract_k_nearest();
        }
      }

      // Extra stop in-case we need to stop because of hdist
      if (stop_query(recall, log2(buckets), found.size(), k, found.get_kth_dist()))
        break;

      // If one map has next bucket they all do, so we just check for an arbitrary map
      if (!this->maps[0]->has_next_bucket(hash[0], hdist, ++mask_index)) {
        ++hdist;
        mask_index = 0;
      }

      buckets++;
    }

    this->stop_found = found.size();
    this->stop_hdist = hdist;
    this->stop_mask_index = mask_index;
    this->buckets_visited = buckets;

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
