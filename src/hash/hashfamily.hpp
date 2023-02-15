#pragma once

#include <vector>
#include <functional>

#include "../index/point.hpp"

template<ui32 D>
using BinaryHash = std::function<bool(const Point<D>&)>;

/**
 * D dimensional Point hash family
 */
template <ui32 D>
class HashFamily : public std::vector<BinaryHash<D>> {
public:
  using std::vector<BinaryHash<D>>::vector;
};

