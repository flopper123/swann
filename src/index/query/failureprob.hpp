#pragma once

#include "../index.hpp"

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

// 3 / (6*3) = 0.2 

// 1-0.8 > 0.2

