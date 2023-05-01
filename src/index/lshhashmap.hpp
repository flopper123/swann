#pragma once

#include "lshmap.hpp"

template <ui32 D>
class LSHHashMap : public LSHMap<D> {
  // all possible masks by hamming distance 
  static inline std::vector<std::vector<ui32>> masks = std::vector<std::vector<ui32>>();
  
  void initMasks() {
    ui32 constructed_masks = 0;
    for (ui32 hdist = 0; hdist <= this->depth(); ++hdist) {
      // The case might be that we have already initialized this hdist, 
      // so we recompute since each time the depth increases new options emerge.
      if (LSHHashMap<D>::masks.size() > hdist) 
      {
        LSHHashMap<D>::masks[hdist] = std::vector<ui32>(); 
      } else {
        LSHHashMap<D>::masks.emplace_back(std::vector<ui32>());
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
        LSHHashMap<D>::masks[hdist].emplace_back(m);
        // Ensure we don't go out of bound
      } while (std::next_permutation(ALL(vmask)));
      
      std::sort(ALL(LSHHashMap<D>::masks[hdist])); // sort ascending to ensure query stays within bound for multiple depths

      constructed_masks += LSHHashMap<D>::masks[hdist].size();
    }

    std::cout << "Masks size: " << this->masks.size() << std::endl
              << "Total masks: " << constructed_masks << std::endl;
  }

public:
  LSHHashMap(HashFamily<D>& hf) : LSHMap<D>(hf)
  {
    this->build(hf);
  }

  /**
   * @brief Initialize this map with the given hashes. After this call, the map will be empty
   * @warning This will reset the map, and all points inserted will be lost
   * @param hf The hashfamily to build the map with
   */
  void build(HashFamily<D>& hf) {
    this->hashes = hf;

    this->buckets.clear();

    count = 0;
    
    this->number_virtual_buckets = 1ULL << this->hashes.size();

    if (masks.size() < this->depth() + 1)
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
  ui32 bucketCount() const { return this->number_virtual_buckets; };

  /**
   * @brief Inserts a point into the map
   */
  void add(const Point<D> &point) {
    const hash_idx index = this->hash(point);

    if (!this->buckets.contains(index)) {
      this->buckets.insert({ index, bucket() });
    }

    buckets[index].emplace_back(count++);
  };

  /**
   * @brief Inserts a vector of points into the map
   */
  void add(std::vector<Point<D>> &points) {
    for (const auto& p : points) {
      this->add(p);
    }

    // this->buckets.rehash(0);
  };

  /**
   * @returns The hash (index) of the bucket the point belongs to
   */
  hash_idx hash(const Point<D>& point) const {
    return this->hashes(point);
  };

  /**
   * @returns Bucket sizes of buckets
   */
  std::vector<ui32> get_bucket_sizes() const {
    std::vector<ui32> sizes;
    for (const auto& [ _, b ] : this->buckets) {
      sizes.emplace_back(b.size());
    }
    return sizes;
  };


  /**
   * @brief Returns true if there is a next bucket with hamming distance of hdist
   * @param bucket 
   * @param hdist 
   * @param mask_idx
   */
  inline bool has_next_bucket(hash_idx bucket, ui32 hdist, ui32 mask_idx) const 
  {
    return mask_idx < LSHHashMap<D>::masks[hdist].size() && LSHHashMap<D>::masks[hdist][mask_idx] < this->bucketCount();
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
    return bucket ^ LSHHashMap<D>::masks[hdist][mask_idx];
  }
  
  /**
   * @param bidx starting index of the bucket
   * @param hdist the hamming distance of the buckets to retrieve 
   * @returns Returns a vector containing all other bucket indices with 
   *          hamming distance of hdist
   */
  std::vector<hash_idx> query(hash_idx bidx, ui32 hdist = 0) const {

    const ui32 MI = LSHHashMap<D>::masks[hdist].size();
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
    // Return empty bucket if the bucket does not exist
    if (!this->buckets.contains(bidx)) {
      return this->empty_bucket;
    }
    
    return this->buckets[bidx];
  }
  
private:
  std::unordered_map<hash_idx, bucket> buckets;
  bucket empty_bucket;
  ui64 count;
  ui32 number_virtual_buckets;
};