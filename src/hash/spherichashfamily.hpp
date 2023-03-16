#pragma once

#include <algorithm>
#include "../index/pointarithmeticresult.hpp"
#include "hashfamily.hpp"
#include <experimental/iterator>


template<ui32 D> 
class PointForce {
public:
  const double threshold = 0.5d;
  double f[D];

  PointForce() { std::memset(f, 0, sizeof(f)); }

  double &operator[](ui32 d) { return f[d]; }

  // Applies the force to the point
  void apply(Point<D>& src){
    //! Modify point
    
    //! Clear forces
    std::memset(f, 0, sizeof(f));
  }
};

template<ui32 D>
class SphericalHashFamily : public HashFamily<D> {
private:
  struct hfargs {
      PointForce<D> force;          // force.toPoint() => f_i
      std::vector<ui32> shared_cnt; // o_ij
      ui32 threshold;
      Point<D> p;
        
      hfargs() : 
        force(), 
        shared_cnt(), 
        threshold(0), 
        p(0x0)
      {}
      
      BinaryHash<D> toLambda() const {
        return ([this](const Point<D> &p2)
                { return this->p.spherical_distance(p2) <= this->threshold; });
      }

      bool apply(const Point<D>& p2) const
      {
        return p.spherical_distance(p2) <= threshold;
      }     
    };

public:
  using HashFamily<D>::HashFamily;
  
  template<typename PointIterator>
  void optimize(ui32 size, PointIterator sample_beg, PointIterator sample_end) {
    // Build optimal hash functions
    // const double err_margin = 0.25;
    const ui32 N = std::distance(sample_beg,sample_end);
    assert(N >= size);

    std::vector<Point<D>> sample(sample_beg, sample_end), // sample in order
                          tmp(size);                      // random subset of samples of size points
    std::sample(ALL(sample), tmp.begin(), size, std::mt19937{std::random_device{}()});
    
    // Choose some random pivot to sort points by
    Point<D> pivot(sample[N/2]); 

    // Sort sample by distance to pivot for each point
    std::sort(ALL(sample), [&pivot](const Point<D> &p1, const Point<D> &p2)
              { return p1.spherical_distance(pivot) < p2.spherical_distance(pivot); });

    // Update pivot to be median of points 
    pivot = sample[N/2]; 

    // Initialize start points of hashfunctions to be the size random sampled points
    std::vector<hfargs> hfs(size);
    for (int i = 0; i < size; ++i)
    {
      auto &p = tmp[i];
      hfs[i].p = p;
      hfs[i].shared_cnt.resize(size);
    }
    
    constexpr ui32 max_rounds = 10;
    double size_4 = size / 4.0;
    ui32 r = 0;
    

    do {
      for (int i = 0; i < size; ++i) {
        // Update points with forces
        hfs[i].force.apply(hfs[i].p);
        
        // Compute new thresholds
        hfs[i].threshold = hfs[i].p.spherical_distance(pivot);
        
        // reset oij
        hfs[i].shared_cnt.assign(size, 0);
      }

      // compute oijs
      for (int i=0; i < size; ++i)
      {
        for (int j=0; j < size; ++j)
        {
          hfs[i].shared_cnt[j] = std::accumulate(ALL(sample), 0, [&hfs, i, j](ui32 acc, const Point<D> &p)
                          { return acc + ((bool)hfs[i].apply(p) & hfs[j].apply(p)); });
        }
      }
      
      //! TO:DO add check if oij is within error margin 
      for (int i = 0; i < size-1; ++i) {
        for (int j = i + 1; j < size; ++j) {
          PointSubtractionResult<D> psub(hfs[i].p,hfs[j].p);

          // Calculate point force for each dimension
          PointForce<D> f_ij;
          for (int d = 0; d < D; ++d) {
            f_ij[d] = 0.5 * ((hfs[i].shared_cnt[j] - size_4) / size_4) * (psub.neg()[d] + psub.pos()[d]);
            hfs[i].force[d] += f_ij[d];
            hfs[j].force[d] -= f_ij[d];
          }
        }
      }
    } while (r++ < max_rounds);
    
    // Add constructed hashfunctions to hashfamily
    std::transform(ALL(hfs), std::back_inserter(*this), 
      [](const hfargs &h){ return h.toLambda(); });
  }

  /**
   * @brief A data dependent constructor for constructing a hashfamily of spherical hash functions
   *        Construct by the iterative optimization algorithm proposed in 
   *        Spherical Hashing: Binary Code Embedding with Hyperspheres
   *        Runs in asymptotitc time complexity of O((l^2 + l*D)*N) where
   *        - N = size of sample points
   *        - l = number of hyperspheres
   *        - D = dimensions of points
   * @param size Number of hyperspheres i.e. number of hash functions  
   * @param sample_beg An iterator pointing to the start of the sample points
   * @param sample_end An iterator pointing to the end of the sample points         
   */
  template<typename PointIterator>
  SphericalHashFamily(ui32 size, 
                      PointIterator sample_beg, PointIterator sample_end)
    : HashFamily<D>(size)
  {
    this->optimize(size, sample_beg, sample_end);
  }

  SphericalHashFamily() : HashFamily<D>() {}
};