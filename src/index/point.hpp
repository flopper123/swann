#pragma once

#include <random>
#include <bitset>
#include "../global.hpp"

// hold as global to allow retrieving for testing
const double point_sp_offset = 0.1d; 

/** Binary vector point */
template<ui32 D>
class Point : public std::bitset<D> {
  public:
    using std::bitset<D>::bitset;

    /** @brief Computes the Hamming distance between two points */
    inline ui32 distance(const Point<D>& p2) const {
      return (*this ^ p2).count();
    }
    /** @brief Computes spherical distance by division */
    inline double spherical_distance(const Point<D>& p2) const {
      // offset to avoid branching for division by zero check
      return (this->distance(p2)) / ((double)(*this & p2).count() + point_sp_offset);
    }
    
    static inline constexpr Point<D> Empty = Point<D>{0x0};
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