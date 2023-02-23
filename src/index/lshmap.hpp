#pragma once

#include "index.hpp"
#include "../hash/hashfamily.hpp"

template<ui32 D>
class LSHMap {
  typedef vec<ui32> bucket; // index bucket
  typedef hash_idx ui32;

  /**
   * @returns Number of hash-functions in the chain
   **/
  virtual ui32 depth() = 0;

  /**
   * Inserts a point into the map
   */
  virtual void add(Point<D> &point) = 0;

  /**
   * Inserts a vector of points into the map
   */
  virtual void add(std::vector<Point<D>> &points) = 0;

  /**
   * @returns The hash (index) of the bucket the point belongs to
   */
  virtual hash_idx hash(const Point<D>& point) = 0;

  /**
   * @param bidx starting index of the bucket
   * @param hdist the hamming distance of the buckets to retrieve 
   * @param k maximum number of matches in the buckets
   * @returns Returns a vector containing all other bucket indices with 
   *          hamming distance of dist
   */
  virtual vec<hash_idx> query(hash_idx bidx, ui32 hdist = 0, ui32 k = UINT32_MAX) = 0;

  /**
   * @returns Returns the bucket at index bidx
   */
  virtual bucket& operator[](hash_idx bidx) = 0;
};

