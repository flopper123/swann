#pragma once

#include "lshmap.hpp"

template <ui32 D>
class LSHArrayMap : public LSHMap<D> {
  // all possible masks by hamming distance 
  static inline std::vector<std::vector<ui32>> masks = std::vector<std::vector<ui32>>();
  
  void initMasks() {
    for (ui32 hdist = 0; hdist <= this->depth(); ++hdist) {
      // The case might be that we have already initialized this hdist, 
      // so we recompute since each time the depth increases new options emerge.
      if (LSHArrayMap<D>::masks.size() > hdist) 
      {
        LSHArrayMap<D>::masks[hdist] = std::vector<ui32>(); 
      } else {
        LSHArrayMap<D>::masks.emplace_back(std::vector<ui32>());
      } 

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
    if (masks.size() < this->depth()+1)
    {
      initMasks();
    }
  }

  /**
   * @returns Number of hash-functions in the chain
   */
  ui32 depth() const { return this->hashes.size(); }; 
  
  /**
   * @returns Number of points in all buckets
   */
  ui32 size() const { return this->count; };

  /**
   * @returns Number of buckets in the map
   */
  ui32 bucketCount() const { return this->buckets.size(); };

  /**
   * @brief Inserts a point into the map
   */
  void add(const Point<D> &point) {
    const hash_idx index = this->hash(point);
    buckets[index].emplace_back(count++);
  };

  /**
   * @brief Inserts a vector of points into the map
   */
  void add(std::vector<Point<D>> &points) {
    for (const auto& p : points) {
      this->add(p);
    }
  };

  /**
   * @returns The hash (index) of the bucket the point belongs to
   */
  hash_idx hash(const Point<D>& point) const {
    return this->hashes(point);
  };

  
  /**
   * @brief Returns true if there is a next bucket with hamming distance of hdist
   * @param bucket 
   * @param hdist 
   * @param mask_idx
   */
  inline bool has_next_bucket(hash_idx bucket, ui32 hdist, ui32 mask_idx) const 
  {
    return LSHArrayMap<D>::masks[hdist][mask_idx] < this->bucketCount() && mask_idx < LSHArrayMap<D>::masks[hdist].size();
  }
  
  /**
   * @brief Returns the next bucket index which has @hdist hamming distance to @bucket.
   *        The search is started at the given mask_idx
   * @param bucket Starting index of the bucket to search from
   * @param hdist The hamming distance of the buckets to retrieve
   * @param mask_idx Starting index of the masks to apply
   * @return hash_idx 
   */
  inline hash_idx next_bucket(hash_idx bucket, ui32 hdist, ui32 mask_idx) const
  {
    return bucket ^ LSHArrayMap<D>::masks[hdist][mask_idx];
  }
  
  /**
   * @param bidx starting index of the bucket
   * @param hdist the hamming distance of the buckets to retrieve 
   * @returns Returns a vector containing all other bucket indices with 
   *          hamming distance of hdist
   */
  std::vector<hash_idx> query(hash_idx bidx, ui32 hdist = 0) const {

    const ui32 MI = LSHArrayMap<D>::masks[hdist].size();
    std::vector<hash_idx> res;
    
    for (ui32 mi = 0; 
         mi < MI && this->has_next_bucket(bidx, hdist, mi); // since bucketCount is a power of 2, we can compare integer values
         ++mi) 
    {
      res.emplace_back(this->next_bucket(bidx, hdist, mi));
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