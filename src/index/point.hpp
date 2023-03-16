#pragma once

#include <random>
#include <bitset>
#include "../global.hpp"

/** Binary vector point */
template<ui32 D>
class Point : public std::bitset<D> {
  public:
    using std::bitset<D>::bitset;
    /** @brief Computes the spherical hamming distance between two points by subtraction */
    inline ui32 spherical_distance(const Point<D>& p2) const {
      return distance(p2) - (*this & p2).count();
    }

    /** @brief Computes the Hamming distance between two points */
    inline ui32 distance(const Point<D>& p2) const {
      return (*this ^ p2).count();
    }

    friend inline operator-(const Point<D>& p1, const Point<D>& p2) : PointArithmeticResult<D> const {
      return distance(p2);
    }
};

template<ui32 D>
class PointArithmeticResult : public Pair<Point<D>, Point<D>> {
public:
  using Pair<Point<D>, Point<D>>::Pair;

  Point<D>& neg() { return this->first; }
  Point<D>& pos() { return this->second; }
};

/** 
 * @brief Generates a random point from a Bernoulli distribution
 * @param p controls the Bernoulli distribution
 */
template<ui32 D> Point<D> random_point(double p = 0.5) {
  Point<D> bits;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::bernoulli_distribution d(p);

  for(int n = 0; n < D; ++n) {
    bits[n] = d(gen);
  }

  return bits;
}