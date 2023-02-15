#pragma once

#include <bitset>
#include <vector>
#include <algorithm> 
#include "../global.hpp"
#include "point.hpp"

template<int D>
class Index {
  public:
    virtual ui32 size() = 0;
    virtual void build() = 0;
    virtual void insert(Point<D>& point) = 0;
    virtual std::vector<ui32> query(const Point<D>& point, int k) = 0;
    virtual Point<D> &operator[](ui32 i) = 0;
};
