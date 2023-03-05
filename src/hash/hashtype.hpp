#pragma once

#include <stdint.h>

// Enum mask for hash functions
enum HashType
 : uint32_t
{
  RndBit = 0b1,
  RndMask = 0b10,
  Lambda = 1UL << 31UL,
};

inline HashType operator~ (HashType a) { return (HashType)~(uint32_t)a; }
inline HashType operator| (HashType a, HashType b) { return (HashType)((uint32_t)a | (uint32_t)b); }
inline HashType operator& (HashType a, HashType b) { return (HashType)((uint32_t)a & (uint32_t)b); }
inline HashType operator^ (HashType a, HashType b) { return (HashType)((uint32_t)a ^ (uint32_t)b); }
inline HashType& operator|= (HashType& a, HashType b) { return (HashType&)((uint32_t&)a |= (uint32_t)b); }
inline HashType& operator&= (HashType& a, HashType b) { return (HashType&)((uint32_t&)a &= (uint32_t)b); }
inline HashType& operator^= (HashType& a, HashType b) { return (HashType&)((uint32_t&)a ^= (uint32_t)b); }