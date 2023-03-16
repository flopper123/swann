#pragma once

#include "point.hpp"

template<ui32 D>
class PointArithmeticResult : public std::pair<Point<D>, Point<D>> {
  virtual void compute(const Point<D>& lhs, const Point<D>& rhs) = 0;

public:
  using std::pair<Point<D>, Point<D>>::pair;
  
  PointArithmeticResult(const Point<D>& lhs, const Point<D>& rhs) {
    this->compute(lhs, rhs);
  }

  Point<D>& pos() { return this->first; }
  Point<D>& neg() { return this->second; }  
};

template<ui32 D>
class PointSubtractionResult : public PointArithmeticResult<D> {
  virtual void compute(const Point<D>& lhs, const Point<D>& rhs) override {
    auto xOR = lhs ^ rhs;
    // Point<D> xOR = *static_cast<Point<D>*>(&bset_XOR);
    auto bset_pos = xOR & lhs;
    this->pos() = *static_cast<Point<D>*>(&bset_pos); // Which bits does lhs have that rhs doesnt have
    auto bset_neg = xOR & rhs;
    this->neg() = *static_cast<Point<D>*>(&bset_neg); // Which bits does rhs have that lhs doesnt have
  }

public: 
  using PointArithmeticResult<D>::PointArithmeticResult;
};