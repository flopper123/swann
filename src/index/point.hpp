#pragma once

#include <bitset>
#include "../global.hpp"

/** Binary vector point */
template<int D>
class Point : std::bitset<D> {
  public:
    using std::bitset<D>::bitset;
    
    // Computes the Hamming distance between to points
    ui32 distance(const Point<D>& p2) {
      return (*this ^ p2).count();
    }
};