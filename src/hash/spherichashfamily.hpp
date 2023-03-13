#pragma once

#include "hashfamily.hpp"
#include <experimental/iterator>

template<ui32 D>
class SphericalHashFamily : public HashFamily<D> {
public:
  using HashFamily<D>::HashFamily;
 
  // A data independent constructor for constructing a hashfamily of size spherical hash functions
  // SphericalHashFamily(ui32 size, ui32 threshold, float distribution_factor = 0.5) {
  //   for (ui32 i = 0; i < size; ++i) {
  //     auto mask = random_point<D>(distribution_factor);
  //     this->push_back([mask, threshold](const Point<D> &p)  
  //                  { return (p.spherical_distance(mask)) < threshold; });  
  //   }
  // }

  /**
   * @brief A data dependent constructor for constructing a hashfamily of spherical hash functions
   *        Construct by the iterative optimization algorithm proposed in 
   *        Spherical Hashing: Binary Code Embedding with Hyperspheres
   *        Runs in asymptotitc time complexity of O((l^2 + l*D)*N) where
   *        - N = size of sample points
   *        - l = number of hyperspheres
   *        - D = dimensions of points
   * @param size Number of hyperspheres i.e. number of hash functions  
   * @param sample A sample of the points in the dataset         
   */
  template<typename Iterator>
  SphericalHashFamily(ui32 size, 
                      Iterator beg, Iterator end)
    : HashFamily<D>(size)
  {
    ui32 N = std::distance(beg,end);
    std::cout << "Received " << N << " points" << std::endl;
    for (Iterator cur = beg; cur != end; ++cur) {
        std::cout << *cur << "\n";
    }
    std::cout << std::endl;
  }
};