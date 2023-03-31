#pragma once

#include "../index.hpp"
#include <numbers>

using QueryFailureProbability = std::function<float(ui32, ui32, ui32, ui32, ui32)>;

/**
 * @param N Number of maps in the forest
 * @param tDepth Maximum depth of each tree in the forest
 * @param depth Current depth of the tree being queried
 * @param found Number of points found so far
 * @param tar Number of kNN to find
*/
static float SimpleSizeFailure(ui32 N, ui32 tDepth, ui32 depth, ui32 found, ui32 tar) 
{
  const float factor = 3;
  found = found ? found : 1;
  return tar / (found * factor);
}

static float TestSizeFailure(ui32 N, ui32 tDepth, ui32 depth, ui32 found, ui32 tar)
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
 */
static float HammingSizeFailure(ui32 N, ui32 tDepth, ui32 depth, ui32 found, ui32 tar)
{
  const float factor = (N * (depth / tDepth) + 1); 
  return (tar*(tDepth-depth+1)*std::numbers::e_v<double>) / (found * factor+0.01);
}