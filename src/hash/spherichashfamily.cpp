#pragma once

#include "hashfamily.hpp"

template<ui32 D>
class SphericalHashFamily : public HashFamily<D> {
public:
  using std::vector<BinaryHash<D>>::vector;
  using HashFamily<D>::HashFamily;
  
  //! TO:DO change to 
  // A hashfamily of size spherical hash functions
  SphericalHashFamily(ui32 size, ui32 threshold, float distribution_factor = 0.5) {
    for (ui32 i = 0; i < size; ++i) {
      auto mask = random_point<D>(distribution_factor);
      this->push_back([mask, threshold](const Point<D> &p)  
                   { return (p.spherical_distance(mask)) < threshold; });  
    }
  }
};