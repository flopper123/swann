#pragma once

#include <vector>
#include <functional>

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
};

