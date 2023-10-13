#pragma once

#include "../index.hpp"
#include "../lshhashmap.hpp"
#include <numbers>

using QueryFailureProbability = std::function<float(ui32, ui32, ui32, ui32, ui32, ui32)>;

/**
 * @param N Number of maps in the forest
 * @param tDepth Maximum depth of each tree in the forest
 * @param depth Current depth of the tree being queried
 * @param found Number of points found so far
 * @param tar Number of kNN to find
 * @param outerCandidateHDist Hamming dist of the outer candidate point
*/
static float SimpleSizeFailure(ui32 N, ui32 tDepth, ui32 depth, ui32 found, ui32 tar, ui32 outerCandidateHDist) 
{
  const float factor = 3;
  found = found ? found : 1;
  return tar / (found * factor);
}

static float TestSizeFailure(ui32 N, ui32 tDepth, ui32 depth, ui32 found, ui32 tar, ui32 outerCandidateHDist)
{
  const float factor = N * depth + 1;
  return tar / ((found+0.01) * factor);
}

/**
 * @brief A failure-probability highly dependent on the current depth of the trie
 * @param N Number of maps in the forest
 * @param tDepth Maximum depth of each tree in the forest
 * @param depth Current depth of the tree being queried
 * @param found Number of points found so far
 * @param tar Number of kNN to find
 * @param outerCandidateHDist Hamming dist of the outer candidate point
 */
static float HammingSizeFailure(ui32 N, ui32 tDepth, ui32 depth, ui32 found, ui32 tar, ui32 outerCandidateHDist)
{
  const ui32 buckets = 1UL << tDepth;
  const double nr_points = std::pow(std::numbers::e_v<double>, tDepth);
  const double found_factor = (buckets*std::numbers::egamma_v<double> / nr_points),
               tar_factor = (std::pow(std::numbers::e_v<double>, depth/((double) tDepth)))*(1.0+(1.0/N));
  return tar*tar_factor / ((found * found_factor) + 0.001);
}

/**
 * @brief A failure-probability on single bit hash functions
 * @param N Number of maps in the forest
 * @param tDepth Maximum depth of each tree in the forest
 * @param depth Current depth of the tree being queried
 * @param found Number of points found so far
 * @param tar Number of kNN to find
 * @param outerCandidateHDist Hamming dist of the outer candidate point
 */
template<ui32 D>
static float SingleBitFailure(ui32 N, ui32 tDepth, ui32 depth, ui32 found, ui32 tar, ui32 outerCandidateHDist)
{
  float R = outerCandidateHDist;

  float p1 = 1.0 - (R / D); // Lowest probability of points being in same bucket

  float L = N;

  // for bottom of trie depth is 0, so we need to subtract depth from tDepth to get actual depth
  float actDepth = depth;

  return std::pow(1.0 - std::pow(p1, tDepth-actDepth), L);
}

/**
 * @brief A failure-probability on single bit hash functions
 * @param L Number of maps in the forest
 * @param l The number of maps checked at hdist depth
 * @param tDepth Maximum depth of each tree in the forest
 * @param depth Current hdist 
 * @param outerCandidateHDist Hamming dist of the outer candidate point to query point
 * @param cur_hdist_buckets The number of buckets checked out of @total at hdist depth
 */
template<ui32 D>
static float SingleBitFailure_Accumulative(
  ui32 L, ui32 l, 
  ui32 tdepth,ui32 depth, ui32 outerCandidateHDist, 
  ui32 cur_hdist_buckets)
{
  // Lowest probability of points being in same bucket
  float p1 = 1.0 - (((float)outerCandidateHDist) / D); 

  // Total number of buckets at current @depth in a trie of @tdepth height
  const ui32 total_hdist_buckets = LSHHashMap<D>::mask_count(tdepth, depth);
  
  // Fraction of buckets checked at current depth
  float bucket_frac = (float)cur_hdist_buckets / (float)total_hdist_buckets,
        d = tdepth-depth-1; 
  
  float l0 = L-l, // Remaning number of maps to check at @depth
        l1 = l;   // Number of maps checked at hdist @depth
  
  float t0 = (depth > 0) ? std::pow(1.0 - std::pow(p1, d), l0) : 1.0,
        t1 = std::pow(1.0 - std::pow(p1, d+bucket_frac), l1);
        
  // std::cout << "p1: " << p1 << std::endl
  //           << "l0: " << l0 << std::endl
  //           << "l1: " << l1 << std::endl
  //           << "d:" << tdepth-depth << std::endl
  //           << "t0: " << t0 << ", t1: " << t1 << std::endl
  //           << "bucket_frac:" << bucket_frac << std::endl
  //           << "total_hdist_buckets:" << total_hdist_buckets << std::endl;

  return t0 * t1;
}