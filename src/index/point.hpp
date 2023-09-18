#pragma once

#include <random>
#include <bitset>
#include "../global.hpp"

/** Binary vector point */
template<ui32 D>
class Point : public std::bitset<D> {
  public:
    using std::bitset<D>::bitset;
    
    Point<D> operator~() const noexcept {
      Point<D> ret(*this);
      ret.flip();
      return ret;
    }

    /** @brief Computes the Hamming distance between two points */
    inline ui32 distance(const Point<D>& p2) const noexcept {
      return (*this ^ p2).count();
    }
    
    /** 
     * @brief Generates a random point from a Bernoulli distribution
     * @param p controls the Bernoulli distribution
     */
    static inline Point<D> random(double p = 0.5) {
      Point<D> bits;
      std::random_device rd;
      std::mt19937 gen(rd());
      std::bernoulli_distribution d(p);

      for(int n = 0; n < D; ++n) {
        bits[n] = d(gen);
      }

      return bits;
    }
    
    static inline constexpr Point<D> Empty = Point<D>{0x0};
};
