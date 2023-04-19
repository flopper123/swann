#pragma once

#include "../../hash/hashfamily.hpp"
#include "../../index/lshtrie.hpp"

constexpr ui32 D = 4;

static inline HashFamily<D> H = {
  [](const Point<D>& p) { return p[0]; },
  [](const Point<D>& p) { return p[1]; },
  [](const Point<D>& p) { return p[2]; },
  [](const Point<D>& p) { return p[3]; },
};

/**
 * @brief Returns a vector containing all possible unique combinations of points
 * @arg 1 <= DIM <= 64 : Dimensions of the points 
 */
template<ui32 DIM>
static inline std::vector<Point<DIM>> createAllPoints() {
  assert(DIM < 64);
  std::vector<Point<DIM>> input;

  // All combinations of input to bitset
  for (ui64 i = 0; i < (1ULL << DIM); ++i) {
    input.emplace_back(i);
  }

  return input;
}

// Returns a vector containing all possible unique combinations of bitvectors
static inline std::vector<Point<D>> createCompleteInput() {
  return createAllPoints<D>();
}
