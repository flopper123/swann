#pragma once

#include "lshmap.hpp"

template <ui32 D>
class LSHArrayMap : public LSHMap<D> {

public:
  LSHArrayMap(HashFamily<D>& hf)
      : LSHMap<D>(hf),
        buckets(
            1ULL << hf.size(), // Number of buckets is 2^number_of_hashes
            bucket()),
        count(0)
  { }

  /**
   * @returns Number of hash-functions in the chain
   */
  ui32 depth() { return this->hashes.size(); };

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
  
    std::vector<hash_idx> res(1, bidx);

    if (hdist == 0) {
      return res;
    }

    ui32 max_depth = this->depth();

    // Flip the d-th bit
    for (ui32 d = 0; d < max_depth; ++d) {
      ui32 b_x = bidx ^ (1ULL << d);
      std::vector<hash_idx> innerRes = this->query(b_x, hdist - 1);
      res.insert(res.end(), ALL(innerRes));
    }

    return res;
  };

  /**
   * @returns Returns the bucket at index bidx
   */
  bucket& operator[](hash_idx bidx) {
    return this->buckets[bidx];
  }

  /** @returns count of inserted points */
  ui32 size() { return count; }
  
private:
  std::vector<bucket> buckets;
  ui64 count;
};