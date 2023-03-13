#pragma once

#include <stdint.h>

// Enum mask for hash functions
enum HashType
 : uint32_t
{
  Bit = 0b1,
  Mask = 0b10,
  Hamming = 0b100,
  Spherical = 0b1000,
  Lambda = 1UL << 31UL,
};

inline HashType operator~ (HashType a) { return (HashType)~(uint32_t)a; }
inline HashType operator| (HashType a, HashType b) { return (HashType)((uint32_t)a | (uint32_t)b); }
inline HashType operator& (HashType a, HashType b) { return (HashType)((uint32_t)a & (uint32_t)b); }
inline HashType operator^ (HashType a, HashType b) { return (HashType)((uint32_t)a ^ (uint32_t)b); }
inline HashType& operator|= (HashType& a, HashType b) { return (HashType&)((uint32_t&)a |= (uint32_t)b); }
inline HashType& operator&= (HashType& a, HashType b) { return (HashType&)((uint32_t&)a &= (uint32_t)b); }
inline HashType& operator^= (HashType& a, HashType b) { return (HashType&)((uint32_t&)a ^= (uint32_t)b); }