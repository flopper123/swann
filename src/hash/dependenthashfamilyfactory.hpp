#pragma once

#include <random>
#include "hashfamily.hpp"
#include "hashtype.hpp"

/** For constructing data dependent hash families */
template<ui32 D>
class DependentHashFamilyFactory {
public:
  /**
   * Creates a hash family of size 'size' using the points in the range [sample_beg, sample_end)
   * Θ(size*N*LogN) where N is the distance from sample_beg to sample_end
   * @param sample_beg - begin iterator of the sample points
   * @param sample_end - end iterator of the sample points
   * @param size - size of the hash family
   */
  template<iterator_to<Point<D>> PointIterator> 
  static inline HashFamily<D> createHDist(PointIterator sample_beg, PointIterator sample_end, ui32 size)
  {
    const ui32 N = std::distance(sample_beg, sample_end);
    assert(N >= size);

    HashFamily<D> HF;
    for (ui32 i = 0; i < size; ++i)
    {
      auto p1 = random_point<D>();
      std::sort(sample_beg, sample_end, [&p1](const Point<D>& a, const Point<D>& b) {
        return p1.distance(a) < p1.distance(b);
      });
      
      // Threshold is the median distance
      const ui32 t = p1.distance(*(sample_beg + (N>>1))); 
      HF.push_back([p1, t](const Point<D> &p2) -> bool {
        return p1.distance(p2) < t;
      });
    }
    return HF;
  }
};
