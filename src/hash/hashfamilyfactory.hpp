#pragma once

#include <random>
#include "hashfamily.hpp"
#include "hashtype.hpp"

template<ui32 D>
class HashFamilyFactory {
public:
  static HashFamily<D> createRandomBits(ui32 size) {
    HashFamily<D> HF;
    for (ui32 bit = 0; bit < size; ++bit) {
      HF.push_back([bit](const Point<D> &p)
                   { return p[bit % D]; });
    }
    return HF;
  }

  static HashFamily<D> createRandomMasks(ui32 size, double distribution_factor = 0.5) {
    HashFamily<D> HF;
    for (ui32 i = 0; i < size; ++i) {
      auto mask = random_point<D>(distribution_factor);
      HF.push_back([mask](const Point<D> &p)
                   { return (p & mask) == mask; });
    }
    return HF;
  }

  static HashFamily<D> createRandomHDist(ui32 size, double distribution_factor = 0.5) {
    HashFamily<D> HF;
    const ui32 fraction = 2;
    for (ui32 i = 0; i < size; ++i)
    {
      auto point = random_point<D>(distribution_factor);
      
      ui32 L = rand() % (D / fraction);
      HF.push_back([point, L](const Point<D> &p)
                   { return p.distance(point) <= L; });
    }
    
    return HF;
  }
};