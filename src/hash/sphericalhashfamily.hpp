#pragma once

#include <algorithm>
#include "../index/pointarithmeticresult.hpp"
#include "hashfamily.hpp"
#include <experimental/iterator>
#include <iostream>

template<ui32 D> 
class PointForce {
public:
  double f[D];

  PointForce() { std::memset(f, 0, sizeof(f)); }

  double &operator[](ui32 d) { return f[d]; }

  /**
   * @brief Transfers the force accumulated from sz points to the point
   *        If f[d] >= threshold       => p[d] = 1 
   *        else if f[d] <= -threshold => p[d] = 0
   * @param p Point to apply force to
   * @param sz Number of points that contributed to the force
   * @param threshold Threshold for applying force to point. 
   */
  void apply(Point<D>& p, const ui32 sz, const double threshold = 0.5d){
    // std::cout << "Applying force to Point{ " << p << " }" << std::endl;
    for (ui32 d = 0; d < D; ++d)
    {
      f[d] = f[d] ? f[d] / sz : 0;
      // std::cout << "\tp[" << d << "] = " << p[d] << " [f=" << f[d] << "]" << std::endl;

      if (f[d] >= threshold || f[d] <= -threshold) {
        // std::cout << "\tFlipping bit " << d << std::endl;
        p[d] = p[d] ^ true;
      }
    }
    // std::cout << "Point after applying force: Point { " << p << " }" << std::endl;
    // Clear forces for next iteration
    std::memset(f, 0, sizeof(f));
  }
};

// Returns a pair of mean and standard deviation of the given vector
static inline double hf_mean(ui32 n, std::vector<std::vector<ui32>>& shared_cnt)
{
  const double n_4 = n/4.0;
  // Accumulator
  double oijAcc = 0.0;
  
  // Accumulate o_i_j - n/4
  for (int i=0; i < shared_cnt.size(); ++i) {
    oijAcc += std::accumulate(ALL(shared_cnt[i]), 0.0, 
      [&n_4](double acc, ui32 val){
        return acc + abs(val - n_4);
    });
  }
  
  return oijAcc / shared_cnt.size();
}

template<ui32 D>
class SphericalHashFamily : public HashFamily<D> {
private:
  struct hfargs {
    PointForce<D> force;   // force.toPoint() => f_i
    Point<D> p;
    ui32 threshold;
      
    hfargs() : force(), threshold(0), p(0x0)
    {}
    
    BinaryHash<D> toLambda() const {
      return ([this](const Point<D> &p2)
              { return this->p.spherical_distance(p2) <= this->threshold; });
    }

    bool apply(const Point<D>& p2) const {
      return p.spherical_distance(p2) <= threshold;
    }     
  };

public:
  using HashFamily<D>::HashFamily;
  
  /** 
   * Build optimal hash functions for sample
   **/
  template<iterator_to<Point<D>> PointIterator>
  void optimize(ui32 size, PointIterator sample_beg, PointIterator sample_end, 
                double err_mean = 0.1, double err_std_dev = 0.15) 
  {
    const ui32 N = std::distance(sample_beg, sample_end);
    assert(N >= size);
    
    constexpr ui32 max_rounds = 10;
    double N_4 = N / 4.0, mean = 0.0, std_dev = 0.0;
    err_mean *= N_4;
    err_std_dev *= N_4;
    ui32 r = 0;

    std::vector<Point<D>> sample(sample_beg, sample_end), // sample in order
                          tmp(size);                      // random subset of samples of size points
    std::sample(ALL(sample), tmp.begin(), size, std::mt19937{std::random_device{}()});

    // Choose some random pivot to sort points by
    Point<D> pivot(sample[N >> 1]);

    // 2d-Array: Number of points within sphere i and j
    std::vector<std::vector<ui32>> shared_cnt(size, std::vector<ui32>(size, 0)); 

    // Sort sample by distance to pivot for each point
    std::sort(ALL(sample), [&pivot](const Point<D> &p1, const Point<D> &p2)
              { return p1.spherical_distance(pivot) < p2.spherical_distance(pivot); });

    // Update pivot to be median of points
    pivot = sample[N >> 1];

    // Initialize start points of hashfunctions to be the size random sampled points
    std::vector<hfargs> hfs(size);
    for (int i = 0; i < size; ++i)
    {
      auto &p = tmp[i];
      hfs[i].p = p;
    }
    
    do {
      // std::cout << "[+] Running round " << r << std::endl;
      for (int i = 0; i < size; ++i)
      {
        // Update points with forces
        hfs[i].force.apply(hfs[i].p, size);
        
        // Compute new thresholds
        hfs[i].threshold = hfs[i].p.spherical_distance(pivot);
        
        // reset oij
        shared_cnt[i].assign(size, 0);
      }

      // compute oijs
      for (int i=0; i < size; ++i) {
        for (int j=0; j < size; ++j) {
          shared_cnt[i][j] = std::accumulate(ALL(sample), 0, [&hfs, i, j](ui32 acc, const Point<D> &p)
                          { return acc + ((bool)hfs[i].apply(p) & hfs[j].apply(p)); });
        }
      }

      mean = hf_mean(N, shared_cnt);
      std_dev = sqrt(Util::variance2D(shared_cnt));
      if (mean <= err_mean && std_dev <= err_std_dev) {
        std::cout << "[+] Found optimal hash functions after " << r << " rounds" << std::endl;
        break;
      }

      for (int i = 0; i < size-1; ++i) {
        for (int j = i + 1; j < size; ++j) {
          auto [pos, neg] = PointCalculator::subtraction<D>(hfs[i].p, hfs[j].p);

          // Calculate point force for each dimension
          PointForce<D> f_ij;
          for (int d = 0; d < D; ++d) {
            f_ij[d] = 0.5 * ((shared_cnt[i][j] - N_4) / N_4) * (pos[d] - neg[d]);
            hfs[i].force[d] += f_ij[d];
            hfs[j].force[d] -= f_ij[d];
          }
        }
      }
    } while (r++ < max_rounds);
    std::cout << "[+] Finished hash function loop in " << r << " rounds" << std::endl;
    // Add constructed hashfunctions to hashfamily
    std::transform(ALL(hfs), std::back_inserter(*this), 
      [](const hfargs &h){ return h.toLambda(); });

    std::cout << "exiting" << std::endl;
  }

  /**
   * @brief A data dependent constructor for constructing a hashfamily of spherical hash functions
   *        Construct by the iterative optimization algorithm proposed in 
   *        Spherical Hashing: Binary Code Embedding with Hyperspheres
   *        Runs in asymptotic time complexity of O((l^2 + l*D)*N) where
   *        - N = size of sample points
   *        - l = number of hyperspheres
   *        - D = dimensions of points
   *        
   *        Let 𝜃_ij denote the number of points in the sample that are in both the i'th and the j'th hypersphere
   *        we consider the sample mean and standard deviation of 𝜃_ij as a measure of the convergence of the 
   *        hashfunctions 
   *        
   *        The algorithm terminates when the mean and standard deviation of 𝜃_ij is within a certain margin of error
   *        defined by parameters err_mean and err_std_dev
   * @param size Number of hyperspheres i.e. number of hash functions  
   * @param sample_beg An iterator pointing to the start of the sample points
   * @param sample_end An iterator pointing to the end of the sample points
   * @param err_mean          
   */
  template<iterator_to<Point<D>> PointIterator>
  SphericalHashFamily(ui32 size, 
                      PointIterator sample_beg, PointIterator sample_end,
                      double err_mean = 0.05, double err_std_dev = 0.06)
    : HashFamily<D>()
  {
    this->optimize(size, sample_beg, sample_end, err_mean, err_std_dev);
  }

  SphericalHashFamily() : HashFamily<D>() {}
};