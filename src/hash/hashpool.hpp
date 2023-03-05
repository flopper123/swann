#pragma once

#include "hashfamily.hpp"
#include "hashfamilyfactory.hpp"

template<ui32 D> 
class HashPool : public HashFamily<D> {
  const double bernoulli_p = 0.5;

public:
  using HashFamily<D>::HashFamily;

  HashPool() : HashFamily<D>() {
    const HashFamily<D> rndBits = HashFamilyFactory<D>::createRandomBits(D),
                        rndMasks = HashFamilyFactory<D>::createRandomMasks(D, bernoulli_p);
    *this += rndBits;
    *this += rndMasks;
  }
};