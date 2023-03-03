#pragma once

#include "../../hash/hashfamily.hpp"
#include "../../index/lshtrie.hpp"

constexpr ui32 D = 4;

static inline HashFamily<D> H = {
  [](const Point<D>& p) { return p[0]; },
  [](const Point<D>& p) { return p[1]; },
  [](const Point<D>& p) { return p[2]; },
  [](const Point<D>& p) { return p[3]; }
};

// Returns a vector containing all possible unique combinations of bitvectors
static inline std::vector<Point<D>> createCompleteInput() {
  std::vector<Point<D>> input;

  // All combinations of input to bitset
  for (ui32 i = 0; i < (1UL << D); ++i) {
    input.push_back(Point<D>(i));
  }

  return input;
}
