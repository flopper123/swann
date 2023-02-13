#pragma once

#include "../global.hpp"
#include "../index/point.hpp"
#include <iostream>
#include <string>
#include <vector>

template <int D>
std::vector<std::pair<std::string, Point<D>>> parse_word_vec(std::istream& is)
{
  ui32 N, d;
  is >> N >> d;
  std::vector<std::pair<std::string, Point<D>>> ret;
  for (int n = 0; n < N; ++n) {
    std::string name;
    std::vector<int> bits(D, false);
    is >> name;
    for (int d = 0; d < D; ++d) {
      is >> bits[d];
    }
    ret.emplace_back(name, bits);
  }
  return ret;
};
