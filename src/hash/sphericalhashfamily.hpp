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
      if (f[d] >= threshold) {
        assert(!p[d]);
        p[d] = true;
      } else if (f[d] <= -threshold)
      {
        assert(p[d]);
        p[d] = false;
      }
    }
    // std::cout << "Point after applying force: Point { " << p << " }" << std::endl;
    // Clear forces for next iteration
    std::memset(f, 0, sizeof(f));
  }
};

// Returns the mean of the given vector
static inline double hf_mean(const ui32 n_4, std::vector<std::vector<ui32>>& shared_cnt)
{
  const int sz = shared_cnt.size();
  // Declare as double to avoid integer division
  double runs = 0, oijAcc = 0; 
  // Accumulate o_i_j - n/4 : avoid visiting the same pair twice
  for (int i=0; i < sz; ++i) 
    for (int j=i+1; j < sz; ++j) 
    {
      oijAcc += std::abs((int) shared_cnt[i][j]) - ((int) n_4);
      ++runs;
    }

  return oijAcc / runs;
}

static inline double hf_std_dev(std::vector<std::vector<ui32>>& shared_cnt) {
  const int size = shared_cnt.size();
  // filter out irrelevant counts - might be a smarter way to do this
  std::vector<double> cnts; 
  for (int i=0; i < size; ++i) {
    for (int j=i+1; j < size; ++j) {
      cnts.emplace_back(shared_cnt[i][j]);
    }
  }
  return Util::std_dev(ALL(cnts));
}

template<ui32 D>
class SphericalHashFamily : public HashFamily<D> {
private:
  struct hfargs {
    PointForce<D> force;  // force.toPoint() => f_i
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
   * @param H Number of hash functions  
   * @param sample_beg An iterator pointing to the start of the sample points
   * @param sample_end An iterator pointing to the end of the sample points
   * @param err_mean Error margin for the mean
   * @param err_std_dev Error margin for the standard deviation
   */
  template<iterator_to<Point<D>> PointIterator>
  void optimize(const ui32 H, PointIterator sample_beg, PointIterator sample_end, 
                double err_mean = 0.1, double err_std_dev = 0.15) 
  {
    ui32 r = 0;
    constexpr ui32 MAX_R = 100;

    // Choose baseline
    const ui32 N = std::distance(sample_beg, sample_end),
             N_4 = N/4; // Number of points in sample
    assert(N >= H && N > 0);
    err_mean *= N_4;
    err_std_dev *= N_4;

    std::vector<Point<D>> sample(sample_beg, sample_end), // sample in order
                          rndPoints(H);                   // random subset of samples of size points
    std::sample(ALL(sample), rndPoints.begin(), H, std::mt19937{std::random_device{}()});

    // Choose some random pivot to sort points by
    Point<D> pivot(sample[N >> 1]);
    // Sort sample by distance to pivot for each point
    std::sort(ALL(sample), [&pivot](const Point<D> &p1, const Point<D> &p2)
              { return p1.spherical_distance(pivot) < p2.spherical_distance(pivot); });
    // Update pivot to be median of points
    pivot = sample[N >> 1];

    // Initialize start points of hashfunctions to be the H random sampled points
    // and initlialize thresholds to be the distance to the pivot
    std::vector<hfargs> hfs(H);
    for (int i = 0; i < H; ++i) {
      hfs[i].p = rndPoints[i];
      hfs[i].threshold = pivot.spherical_distance(hfs[i].p);
    }

    std::vector<std::vector<ui32>> oij(H, std::vector<ui32>(H, 0));
    while (r++ < MAX_R)
    {
      // update all oijs
      for (int i = 0; i < H; ++i) {
        for (int j = i+1; j < H; ++j) {
          // Count number of points in sample that are in both the i'th and the j'th hypersphere
          oij[i][j] = oij[j][i] = std::accumulate(ALL(sample), 0, [&hfs, i, j](ui32 acc, const Point<D> &p)
                                                  { return acc + ((bool)hfs[i].apply(p) & hfs[j].apply(p)); });
        }
      }

      double mean = hf_mean(N_4, oij), std_dev = hf_std_dev(oij);
      if (mean <= err_mean && std_dev <= err_std_dev)
      {
        std::cout << "[+] Found optimal hash functions after " << r << " rounds" << std::endl;
        break;
      }

      // Calculate forces for i..j
      for (int i = 0; i < H; ++i) 
      {
        for (int j = i + 1; j < H; ++j) 
        {
          // oij factor: should we move points closer or further away
          double factor = 0.5 * ((oij[i][j] - N_4) / ((double) N_4)); 
          auto pj = factor <= 0 ? hfs[j].p : ~hfs[i].p;
          auto [pos, neg] = PointCalculator::subtraction<D>(hfs[i].p, pj);
          for (int d = 0; d < D; ++d) {
            hfs[i].force[d] += factor * (pos[d] - neg[d]); // Apply the force i<-j
            hfs[j].force[d] -= hfs[i].force[d];            // Apply the inverse to j<-i
          }
        }
      }
      for (int i = 0; i < H; ++i) {
        hfs[i].force.apply(hfs[i].p, H-1);
        hfs[i].threshold = pivot.spherical_distance(hfs[i].p);
      }
    }

    // Loop until convergence
    std::transform(ALL(hfs), std::back_inserter(*this), 
                    [](const hfargs &h){ return h.toLambda(); });
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