#pragma once

#include <random>
#include <bitset>
#include "../global.hpp"

/** Binary vector point */
template<ui32 D>
class OldPoint : public std::bitset<D> {
  public:
    using std::bitset<D>::bitset;
    
    OldPoint<D> operator~() const noexcept {
      OldPoint<D> ret(*this);
      ret.flip();
      return ret;
    }

    /** @brief Computes the Hamming distance between two points */
    inline ui32 distance(const OldPoint<D>& p2) const noexcept {
      return (*this ^ p2).count();
    }
   
    static inline constexpr OldPoint<D> Empty = OldPoint<D>{0x0};
};

/** 
 * @brief Generates a random point from a Bernoulli distribution
 * @param p controls the Bernoulli distribution
 */
template<ui32 D> OldPoint<D> random_point(double p = 0.5) {
  OldPoint<D> bits;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::bernoulli_distribution d(p);

  for(int n = 0; n < D; ++n) {
    bits[n] = d(gen);
  }

  return bits;
}