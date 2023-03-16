#pragma once

#include "../global.hpp"

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
    case 1: return *beg;
    default: return std::accumulate(beg, end, 0.0) / std::distance(beg, end);
  }
}


/**
 * @brief Computes the variance of a range of values. 
 *        The value type pointed to by iterator must be implicitly castable to double
 * @param beg Iterator pointing to the first element
 * @param end Iterator pointing to the last element
*/
template <iterator TIterator>
static inline double variance(TIterator beg, TIterator end) {
  const size_t N = std::distance(beg, end);
  if (N <= 1) return 0.0;

  // Calculate the mean
  const double m = mean(beg,end);

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
