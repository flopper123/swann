#pragma once
#include <vector>
#include <iostream>
#include <functional>
#include <sstream>

#include "../global.hpp"

/** Mask of next buckets */
class BucketMask : public std::vector<std::vector<ui32>> {
public:
  BucketMask(ui32 depth = 32U, ui32 max_hdist = 4U) : std::vector<std::vector<ui32>>() {
    ui32 constructed_masks = 0;
    if (max_hdist > depth) max_hdist = depth;
    for (ui32 hdist = 0; hdist <= max_hdist; ++hdist) {
      
      this->emplace_back(std::vector<ui32>());
      // Initalize a bitset of size D with hdist bits set
      std::vector<bool> vmask(depth, false);
      
      // Set n first bits to true
      std::fill(vmask.begin(), vmask.begin() + hdist, true);
      std::sort(ALL(vmask));
      
      // create ui32 masks
      do {
        // Transform vmask to ui32
        ui32 m = std::accumulate(vmask.rbegin(), vmask.rend(), 0, [](ui32 x, ui32 y) { return (x << 1UL) + y; });
        this->at(hdist).emplace_back(m);

        // Ensure we don't go out of bound
      } while (std::next_permutation(ALL(vmask)));
      
      std::sort(ALL(this->at(hdist))); // sort ascending to ensure query stays within bound for multiple depths

      constructed_masks += this->at(hdist).size();
    }

  };

};
