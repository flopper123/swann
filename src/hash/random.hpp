#pragma once

#include <random>
#include "hashfamily.hpp"

/** 
 * @brief BinaryHashFunction - Checks if a random bit is set in the point
 **/
template<ui32 D> 
class RandomBit {
  const ui32 bit;

public:
  RandomBit(ui32 i) : bit(i) { assert(i >= 0 && i < D); }
  RandomBit() : bit(rand() % D) { }
  
  bool operator()(const Point<D>& p) const {
    return p[bit];
  }
};



/** 
 * @brief BinaryHashFunction: 
 *        Checks if a random bit is set in the point:
 *        Randomly select K dimensions
 *        Create random bitset mask of length D with K random bits set
 *        Check (point) => point & mask == mask
 **/
template<ui32 D>
class RandomMask {
  const Point<D> mask;

public:
  RandomMask(double distribution_factor = 0.5) : mask(random_point<D>(distribution_factor)) 
  {}
  
  bool operator()(const Point<D>& p) const {
    return (p & mask) == mask;
  }
};