#pragma once

#include "index.hpp"
#include "../hash/hashfamily.hpp"

typedef std::vector<ui32> bucket; // index bucket
typedef ui32 hash_idx;

//! Implement functions with const flag 
template<ui32 D>
class LSHMap {

public:
  HashFamily<D> hashes;

  LSHMap(HashFamily<D>& hashFamily) : hashes(hashFamily) {}

  virtual void build(HashFamily<D>& hashFamily) = 0;

  /**
   * @returns Number of points in the map
   */
  virtual ui32 size() const = 0;

  /**
   * @returns Number of hash-functions in the chain
   */
  virtual ui32 depth() const = 0;

  /**
   * @returns Number of buckets in the map
   */
  virtual ui32 bucketCount() const = 0;

  /**
   * Inserts a point into the map
   */
  virtual void add(const Point<D> &point) = 0;

  /**
   * Inserts a vector of points into the map
   */
  virtual void add(std::vector<Point<D>> &points) = 0;

  /**
   * @returns The hash (index) of the bucket the point belongs to
   */
  virtual hash_idx hash(const Point<D>& point) const = 0;

  /**
   * @returns Returns true if there is a next bucket with hamming distance of hdist
   */
  virtual inline bool has_next_bucket(hash_idx bucket, ui32 hdist, ui32 mask_idx) const = 0;

  /**
   * @returns Returns bucket distribution
   */
  virtual std::vector<ui32> get_bucket_sizes() const = 0;

  /**
   * @brief Returns the next bucket index with hamming distance of hdist
   * @param bucket 
   * @param hdist 
   * @param mask_idx 
   * @return hash_idx 
   */
  virtual inline hash_idx next_bucket(hash_idx bucket, ui32 hdist, ui32 mask_idx) const = 0;

  /**
   * @param bidx starting index of the bucket
   * @param hdist the hamming distance of the buckets to retrieve
   * @returns Returns a vector containing all other bucket indices with 
   *          hamming distance of dist
   */
  virtual std::vector<hash_idx> query(hash_idx bidx, ui32 hdist = 0) const = 0;

  /**
   * @returns Returns the bucket at index bidx
   */
  virtual bucket& operator[](hash_idx bidx) = 0;
};

