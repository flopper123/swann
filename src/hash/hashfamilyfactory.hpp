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
  
  static HashFamily<D> createHDist(ui32 size, ui32 threshold, double distribution_factor = 0.5) {
    HashFamily<D> HF;
    ui32 x = threshold;
    for (ui32 i = 0; i < size; ++i)
    {
      auto point = random_point<D>(distribution_factor);
      HF.push_back([point, x](const Point<D> &p)
                   { return p.distance(point) <= x; });
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

  /**
   * @brief Create a HashFamily containing 'size' hash functions chosen among all the 
   *        possible hash functions given by flags.
   *        if size is not divisible by the number of hash functions, the remainder will be 
   *        excluded.
   * @param size Number of hash functions to create
   * @param flags Flags indicating which hash functions to use
   * ! Consider making distribution of sizes random
  */
  static HashFamily<D> create(ui32 size, HashType flags) {
    HashFamily<D> HF;
    int bits = std::bitset<64>(flags).count();
    size /= bits; // Construct equal number of each type of hash functions
    if (flags & HashType::Bit) {
      HF += createRandomBits(size);
    }
    if (flags & HashType::Mask) {
      HF += createRandomMasks(size);
    }
    if (flags & HashType::Hamming) {
      HF += createRandomHDist(size);
    }
    return HF;
  }
};