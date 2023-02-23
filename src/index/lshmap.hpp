#pragma once

#include "index.hpp"
#include "../hash/hashfamily.hpp"

typedef std::vector<ui32> bucket; // index bucket
typedef ui32 hash_idx;

template<ui32 D>
class LSHMap {

  /**
   * @returns Number of hash-functions in the chain
   **/
  virtual ui32 depth() = 0;

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
  virtual hash_idx hash(const Point<D>& point) = 0;

  /**
   * @param bidx starting index of the bucket
   * @param hdist the hamming distance of the buckets to retrieve
   * @returns Returns a vector containing all other bucket indices with 
   *          hamming distance of dist
   */
  virtual std::vector<hash_idx> query(hash_idx bidx, ui32 hdist = 0) = 0;

  /**
   * @returns Returns the bucket at index bidx
   */
  virtual bucket& operator[](hash_idx bidx) = 0;
};

