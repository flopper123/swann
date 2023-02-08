#pragma once

#include <bitset>

/** Binary vector point */
template<int D>
class Point : std::bitset<D> {
  public:
    using std::bitset<D>::bitset;
  
    // Computes the Hamming distance between to points
    friend inline int distance(const Point<D>& p1, const Point<D>& p2) {
      return (p1 ^ p2).count();
    }
};
