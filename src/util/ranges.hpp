#pragma once

#include "../global.hpp"
#include <iostream>

namespace Util {
  /**
   * @brief Computes the mean of a range of values. 
   *        The value type pointed to by iterator must be implicitly castable to double
   * @param beg Iterator pointing to the first element
   * @param end Iterator pointing to the last element
  */
  template <iterator TIterator>
  static inline double mean(TIterator beg, TIterator end) {
    const size_t N = std::distance(beg, end);
    switch (N) {
      case 0: return 0.0;
      case 1: return (double) *beg;
      default: return std::accumulate(beg, end, 0.0) / N;
    }
  }
  template<typename T>
  static inline double mean2D(std::vector<std::vector<T>>& in) {
    return std::accumulate(ALL(in), 0.0, [](double acc, const auto& row) {
      return acc + mean(ALL(row));
    }) / in.size();
  }
  /**
   * @brief Computes the sample variance of a range of values. 
   *        The value type pointed to by iterator must be implicitly castable to double
   * @param beg Iterator pointing to the first element
   * @param end Iterator pointing to the last element
  */
  template <iterator TIterator>
  static inline double variance(TIterator beg, TIterator end) {
    const size_t N = std::distance(beg, end);
    if (N <= 1) return 0.0;
    // Calculate the mean
    const double m = mean(beg, end);
    // Now calculate the variance
    return std::accumulate(beg, end, 0.0, [&m, &N](double acc, const auto& val) {
      return acc + (val - m)*(val - m) / (N - 1);
    });
  }

  /**
   * @brief Computes the standard deviation of a range of values. 
   *        The template parameter must be implicitly castable to double
   * @param beg Iterator pointing to the first element
   * @param end Iterator pointing to the last element
  */
  template <iterator TIterator>
  static inline double std_dev(TIterator beg, TIterator end) {
    return std::sqrt(variance(beg, end));
  }

  template <iterator TIterator>
  static inline double norm_std_dev(TIterator beg, TIterator end)
  {
    return std_dev(beg, end) / mean(beg, end);
  }

  // Note: I am too lazy to write a proper iterator for this
  template<typename T>
  static inline double variance2D(std::vector<std::vector<T>>& in) {
    const ui32 N = in.size();

    // Flatten matrix
    std::vector<ui32> flat;
    for (ui32 i = 0; i < N; ++i) {
      flat.insert(flat.end(), ALL(in[i]));
    }
    return variance(ALL(flat));
  }
}