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
   *
   * @param sample_beg - begin iterator of the sample points
   * @param sample_end - end iterator of the sample points
   * @param size - size of the hash family
   */
  template<iterator_to<Point<D>> PointIterator> 
  static inline HashFamily<D> createHDist(PointIterator sample_beg, PointIterator sample_end, ui32 size)
  {
    const ui32 N = std::distance(sample_beg, sample_end);
    assert(N >= size);
    std::vector<Point<D>> sample(sample_beg, sample_end);
    Point<D> pivot = sample[N >> 1];
    std::sort(sample.begin(), sample.end(), [&pivot](const Point<D>& a, const Point<D>& b) {
      return pivot.distance(a) < pivot.distance(b);
    });
    pivot = sample[N >> 1]; // update pivot to be median of sorted points

    std::vector<Point<D>> hf_points;
    std::sample(sample.begin(), sample.end(), std::back_inserter(hf_points), size, std::mt19937{std::random_device{}()});
    HashFamily<D> HF;
    for (ui32 i = 0; i < size; ++i)
    {
      auto p1 = hf_points[i];
      ui32 t = pivot.distance(p1);
      HF.push_back([&p1, t](const Point<D> &p2) -> bool {
        return p1.distance(p2) < t;
      });
    }
    return HF;
  }
};
