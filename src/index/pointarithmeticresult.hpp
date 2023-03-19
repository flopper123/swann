#pragma once

#include "point.hpp"

template<ui32 D>
class PointArithmeticResult : public std::pair<Point<D>, Point<D>> {
  virtual void compute(const Point<D>& lhs, const Point<D>& rhs) = 0;

public:
  using std::pair<Point<D>, Point<D>>::pair;
  
  PointArithmeticResult(const Point<D>& lhs, const Point<D>& rhs) 
    : std::pair<Point<D>,Point<D>>(Point<D>::Empty, Point<D>::Empty)
  {
    this->compute(lhs, rhs);
  }
  
  /** @brief Returns a bitset where the bits that are in lhs but not in rhs is set */
  Point<D>& pos() { return this->first; }

  /** @brief Returns a bitset where the bits that are in rhs but not in lhs is set */
  Point<D>& neg() { return this->second; }  
};

template<ui32 D>
class PointSubtractionResult : public PointArithmeticResult<D> {
  virtual void compute(const Point<D>& lhs, const Point<D>& rhs) override {
    auto xOR = lhs ^ rhs;
    auto bset_pos = xOR & lhs;
    auto bset_neg = xOR & rhs;
    
    this->first = *static_cast<Point<D>*>(&bset_pos); // Which bits does lhs have that rhs doesnt have
    this->second = *static_cast<Point<D>*>(&bset_neg); // Which bits does rhs have that lhs doesnt have
  }

public: 
  using PointArithmeticResult<D>::PointArithmeticResult;
};
