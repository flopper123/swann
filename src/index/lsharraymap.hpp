#pragma once

#include "lshmap.hpp"

template <ui32 D>
class LSHArrayMap : public LSHMap<D> {
  // all possible masks by hamming distance 
  static inline std::vector<std::vector<ui32>> masks = std::vector<std::vector<ui32>>();
  
  void initMasks() {
    for (ui32 hdist = masks.size(); hdist <= this->depth(); ++hdist) {
      LSHArrayMap<D>::masks.push_back(std::vector<ui32>());
      // Initalize a bitset of size depth with hdist bits set
      std::vector<bool> vmask(this->depth(), false);
      
      // Set n first bits to true
      std::fill(vmask.begin(), vmask.begin() + hdist, true);
      std::sort(ALL(vmask));
      
      // create ui32 masks
      do {
        // Transform vmask to ui32
        ui32 m = accumulate(vmask.rbegin(), vmask.rend(), 0, [](ui32 x, ui32 y) { return (x << 1UL) + y; });
        LSHArrayMap<D>::masks[hdist].emplace_back(m);
        // Ensure we don't go out of bound
      } while (std::next_permutation(ALL(vmask)));
      
      std::sort(ALL(LSHArrayMap<D>::masks[hdist])); // sort ascending to ensure query stays within bound for multiple depths
    }
  }


public:
  LSHArrayMap(HashFamily<D>& hf)
      : LSHMap<D>(hf),
        buckets(
            (1ULL << hf.size()), // Number of buckets is 2^number_of_hashes
            bucket()),
        count(0)
  {
    
    initMasks();
  }

  /**
   * @returns Number of hash-functions in the chain
   */
  ui32 depth() { return this->hashes.size(); }; 
  
  /**
   * @returns Number of points in all buckets
   */
  ui32 size() { return this->count; };

  /**
   * @returns Number of buckets in the map
   */
  ui32 bucketCount() { return this->buckets.size(); };

  /**
   * Inserts a point into the map
   */
  void add(const Point<D> &point) {
    const hash_idx index = this->hash(point);
    buckets[index].emplace_back(count++);
  };

  /**
   * Inserts a vector of points into the map
   */
  void add(std::vector<Point<D>> &points) {
    for (const auto& p : points) {
      this->add(p);
    }
  };

  /**
   * @returns The hash (index) of the bucket the point belongs to
   */
  hash_idx hash(const Point<D>& point) {
    return this->hashes(point);
  };

  /**
   * @param bidx starting index of the bucket
   * @param hdist the hamming distance of the buckets to retrieve 
   * @returns Returns a vector containing all other bucket indices with 
   *          hamming distance of hdist
   */
  std::vector<hash_idx> query(hash_idx bidx, ui32 hdist = 0) {

    const ui32 MI = LSHArrayMap<D>::masks[hdist].size();
    std::vector<hash_idx> res;
    
    for (ui32 mi = 0; 
         mi < MI && LSHArrayMap<D>::masks[hdist][mi] < this->bucketCount(); // since bucketCount is a power of 2, we can compare integer values
         ++mi) {
      res.emplace_back(bidx ^ LSHArrayMap<D>::masks[hdist][mi]);
    }

    return res;
  };

  /**
   * @brief Retrieve the bucket at the specified bucket-index
   */
  bucket& operator[](hash_idx bidx) {
    
    assert(bidx < this->buckets.size());
    return this->buckets[bidx];
  }
  
private:
  std::vector<bucket> buckets;
  ui64 count;
};