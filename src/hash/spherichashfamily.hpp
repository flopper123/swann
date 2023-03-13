#pragma once

#include <algorithm>
#include "hashfamily.hpp"
#include <experimental/iterator>

template<ui32 D>
class SphericalHashFamily : public HashFamily<D> {
private:
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
    Point<D> dist_pivot = *(beg + N / 2);
    assert(N >= size);
    std::cout << "Received " << N << " points" << std::endl;
    vec<Point<D>> sample(size);
    std::sample(ALL(in), ALL(sample), size, std::mt19937{std::random_device{}()});

    std::vector<Point<D>, ui32> fargs;
    for (int i = 0; i < size; ++i)
    {
      ui32 threshold = dist_pivot->spherical_distance(p[i]);
      fargs.push_back({sample[i], threshold});
    }

    std::vector<int> hfcnt(size, 0); // hfcnt[i] how many points evaluate to true when applying the i'th hash function 
    std::vector<std::vector<int>> hfcntpairs(size, std::vector<int>(size, 0)); // hfcntpairs[i][j] : how many points evaluate to true for i'th and j'th hash function 
    for (ui32 i = 0; i < size; ++i)
    {
      auto &[p1, t] = fargs[i];
      auto shash = [&p1, t](const Point<D> &p2)
      {
        return p1.spherical_distance(p2) < t;
      };

      hfcnt[i] = std::accumulate(ALL(sample), 0, [shash](int acc, const Point<D> &p)
                                  { return acc + shash(p); });

      for (ui32 j = 0; j < size; ++j)
      {
        if (i == j) hfcntpairs[i][j] = hfcnt[i];
        auto shash_j = [&fargs[j]](const Point<D> &p)
        {
          auto &[p1, t] = fargs[j];
          return p1.spherical_distance(p) < t;
        };

        hfcntpairs[i][j] = std::accumulate(ALL(sample), [shash_j, shash](int acc, const Point<D> &p)
                                           { return acc + (shash_j(p) & shash(p)); });
      }
    }      
    std::cout << std::endl;
  }
};