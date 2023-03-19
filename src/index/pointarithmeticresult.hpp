#pragma once
#include <iostream>
#include "point.hpp"

template<ui32 D>
class PointArithmeticResult : public std::pair<Point<D>, Point<D>> {
public:
  using std::pair<Point<D>, Point<D>>::pair;

  /** @brief Returns a bitset where the bits that are in lhs but not in rhs is set */
  Point<D>& pos() { return this->first; }

  /** @brief Returns a bitset where the bits that are in rhs but not in lhs is set */
  Point<D>& neg() { return this->second; }  
};

class PointCalculator {
public:   
  template<ui32 D>
  static inline PointArithmeticResult<D> subtraction(const Point<D>& lhs, const Point<D>& rhs) {
    auto xOR = lhs ^ rhs;
    auto bset_pos = xOR & lhs;
    auto bset_neg = xOR & rhs;
    
    // Which bits does lhs have that rhs doesnt have
    return PointArithmeticResult<D>{
      *static_cast<Point<D>*>(&bset_pos),
      *static_cast<Point<D>*>(&bset_neg)
    }; 
  }
};
