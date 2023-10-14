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
 * @param tDepth Maximum depth of each tree in the forest
 * @param depth Current depth of the tree being queried as a float, to allow for fractional depths
 * @param outerCandidateHDist Hamming dist of the outer candidate point
 */
template<ui32 D>
static float SingleBitFailure_V1(ui32 L, ui32 tDepth, float depth, ui32 outerCandidateHDist)
{
  // Lowest probability of points being in same bucket
  float p1 = 1.0 - (((float) outerCandidateHDist) / D); 

  // for bottom of trie depth is 0, so we need to subtract depth from tDepth to get actual depth
  return std::pow(1.0 - std::pow(p1, tDepth-depth), (float) L);
}

/**
 * @brief A failure-probability on single bit hash functions that accumulates 
 *        the probability of each bucket being checked. This allows us to check 
 *        a fraction of the buckets and tries at each depth
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
  // Fraction of buckets checked at current depth
  float bucket_frac = (cur_hdist_buckets > 0) ? ((float)cur_hdist_buckets) / ((float)LSHHashMap<D>::mask_count(tdepth, depth)) : 0.0; 
  float prv_frac = (cur_hdist_buckets > 1) ? ((float)cur_hdist_buckets-1) / ((float)LSHHashMap<D>::mask_count(tdepth, depth)) : 0.0;

  // Probability of points being in same bucket for tries checked up to newest bucket exclusive
  float prv = (depth > 0 && L > l) ? SingleBitFailure_V1<D>(L-l, tdepth, depth+prv_frac, outerCandidateHDist) : 1.0;

  // Tries checked cur_hdist_buckets inclusive
  float cur = SingleBitFailure_V1<D>(l, tdepth, depth+bucket_frac, outerCandidateHDist);
  if (cur < 0 || prv < 0 || bucket_frac < 0 || prv_frac < 0) {
    std::cout << "cur: " << cur << "\n"
              << "prv: " << prv << "\n"
              << "tdepth: " << tdepth 
              << "depth: " << depth
              << "bucket_frac: " << bucket_frac << "\n"
              << "prv_frac: " << prv_frac << "\n"
              << "mask_count: " << ((float)LSHHashMap<D>::mask_count(tdepth, depth)) << "\n";
  }
  return prv*cur;
}