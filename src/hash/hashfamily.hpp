#pragma once

#include <vector>
#include <functional>
#include "../util/ranges.hpp"
#include "../index/point.hpp"

template<ui32 D>
using BinaryHash = std::function<bool(const Point<D>&)>;

/**
 * D dimensional Point hash family
 */
template <ui32 D>
class HashFamily : public std::vector<BinaryHash<D>> {
public:
  using std::vector<BinaryHash<D>>::vector;
  
  HashFamily& operator+=(const HashFamily& rhs) {
    this->insert(this->end(), ALL(rhs));
    return *this;
  }

  /**
   * @brief Compute the mean of this hash family on the given input points
   *        The mean is defined as the average fraction of 
   *        points that evaluate to true for a hashfunction
   * @param beg Iterator to the first point
   * @param end Iterator to the last point
   */
  template<iterator_to<Point<D>> PointIterator>
  double mean(PointIterator beg, PointIterator end) const {
    if (this->empty()) return 0.0;
    const ui64 N = std::distance(beg, end), H = this->size();
    std::vector<ui32> cnt(H, 0);
    for (int i = 0; i < H; i++) {
      cnt[i] = std::accumulate(beg, end, 0, [this, i](ui32 acc, const Point<D> &p)
                      { return acc + (*this)[i](p); }) / ((double) N);
    }
    return Util::mean(ALL(cnt));
  }

  /**
   * @brief Compute the pair-wise mean of this hash family on the given input points
   *        The pair-wise mean is defined as the average fraction of 
   *        points that evaluate to true for all combinations of hashfunction i and j
   * @param beg Iterator to the first point
   * @param end Iterator to the last point
   */
  template<iterator_to<Point<D>> PointIterator>
  double pairwise_mean(PointIterator beg, PointIterator end) const {
    if (this->empty()) return 0.0;
    const ui64 N = std::distance(beg, end), H = this->size();
    std::vector<std::vector<ui32>> cnt(H, std::vector<ui32>(H, 0));
    for (int i = 0; i < H; i++) {
      for (int j = 0; j < H; ++j) {
        cnt[i][j] = std::accumulate(beg, end, 0, [this, i, j](ui32 acc, const Point<D> &p)
                        { return acc + ((*this)[i](p) & (*this)[j](p)); }) / ((double) N);
      }
    }
    return Util::mean2D(cnt);
  }
 
  /**
   * @brief Compute the spread of this hash family on the given input points
   *        The spread is defined by std_dev(o_ij) where o_ij is the fraction of 
   *        points that evaluate to true for both hashfunction i and j
   * @param beg Iterator to the first point
   * @param end Iterator to the last point
   */
  template<iterator_to<Point<D>> PointIterator>
  double spread(PointIterator beg, PointIterator end) const {
    if (this->empty()) return 0.0;

    const ui64 N = std::distance(beg, end), H = this->size();
    std::vector<std::vector<ui32>> cnt(H, std::vector<ui32>(H, 0));
    for (int i = 0; i < H; i++) {
      for (int j = 0; j < H; ++j) {
        cnt[i][j] = std::accumulate(beg, end, 0, [this, i, j](ui32 acc, const Point<D> &p)
                        { return acc + ((bool)(*this)[i](p) & (*this)[j](p)); });
      }
    }
    
    return sqrt(Util::variance2D(cnt));
  }
    
  /** 
   * @brief Apply all hashes in chain
   * @returns an unsigned 64 bit integer where the i'th bit 
   *          denotes the result of applying the i'th Binary Hash function 
   **/
  ui64 operator()(const Point<D>& p) const {
    assert(this->size() <= 64); // assert to avoid overflowing
    const ui64 depth = this->size();
    ui64 ret = 0x0;
    for (ui64 i = 0; i < depth; ++i)
      ret |= ((ui64) (*this)[i](p)) << i;
    return ret;
  }
  
  //! TO:DO consider changing to std::sample
  /**
   * @param depth the number of hashfamilies to sample
   * @returns A subset hash family containing @depth randomly chosen 
   *          hash functions from this.
   **/
  HashFamily<D> subset(ui32 depth) {
    assert(this->size() >= depth);
    
    // Select random indices
    std::vector<ui32> indices(this->size());
    std::iota(indices.begin(), indices.end(), 0);
    std::random_shuffle(indices.begin(), indices.end());

    // Select subset
    HashFamily<D> ret;
    for (ui32 i = 0; i < depth; ++i) {
      ret.push_back((*this)[indices[i]]);
    }
    return ret;
  }
  
};

