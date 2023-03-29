#pragma once
#include <iostream>
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
    const ui64 N = std::distance(beg, end), H = this->size();
    if (this->empty() || (N == 0)) return 0.0;
    std::vector<double> cnt(H, 0);
    for (int i = 0; i < H; i++) {
      cnt[i] = std::accumulate(beg, end, 0, [this, i](double acc, const Point<D> &p)
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
    if (this->empty()) {
      std::cout << "I am empty :()" << std::endl;
      return 0.0;
    }
    const ui64 N = std::distance(beg, end), H = this->size();
    std::vector<double> cnt;
    
    for (int i = 0; i < H; i++) {
      for (int j = i+1; j < H; ++j) {
        if (i == j) continue; // skip self in calculation
        double oij = std::accumulate(beg, end, 0.0, [this, i, j](double acc, const Point<D> &p)
                                    { return acc + ((int) (((*this)[i](p)) & ((*this)[j](p)))); }) 
                     / ((double) N);
        cnt.push_back(oij);
      }
    }

    return Util::mean(ALL(cnt));
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
    std::vector<double> cnt;
    for (int i = 0; i < H; i++) {
      for (int j = i+1; j < H; ++j) {
        double c = std::accumulate(beg, end, 0.0, [this, i, j](double acc, const Point<D> &p)
                        { return acc + ((int)((bool)(*this)[i](p) & (*this)[j](p))); }) / ((double) N);
        cnt.push_back(c);
      }
    }
    
    return sqrt(Util::variance(ALL(cnt)));
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
  
  /** 
   * @brief Construct a string containing stats of the hashfamily on the input range 
   * @param beg Iterator to the first point
   * @param end Iterator to the last point
  */
  template<iterator_to<Point<D>> PointIterator>
  const std::string str(PointIterator beg, PointIterator end) {
    std::stringstream ss;
    ss << "HashFamily[" << this->size() << "]\n"
       << "\tmean:" << this->mean(beg, end) << "\n"
       << "\tpairwise_mean:" << this->pairwise_mean(beg, end) << "\n"
       << "\tspread:" << this->spread(beg, end) << "\n";
    return ss.str();
  }
  
  /**
   * @brief Expands the current HashFamily by merging hash functions at random.
   *        This is useful for hash families where the number of hash functions is limnited
  */
  HashFamily<D>& expand(ui32 sz = 0) {
    if (sz == 0) sz = this->size();    
    HashFamily<D> res;
    for (ui32 i = 0; i < sz; ++i) {
      // pick 4 hash functions at random
      HashFamily<D> hf_sample = this->subset(4); 
      res.emplace_back([&hf_sample](const Point<D> &p)
        { return (hf_sample[0](p) & hf_sample[1](p)) 
               | (hf_sample[2](p) & hf_sample[3](p)); });
    }
    std::copy(ALL(res), this->begin());
    return *this;
  }
};

