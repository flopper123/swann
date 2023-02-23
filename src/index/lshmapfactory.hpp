#pragma once

#include "../hash/hashfamily.hpp"

template<ui32 D> 
class LSHMapFactory {
private: 
  LSHMapFactory() {}

public:
  static LSHMap<D> create(HashFamily<D>& H, ui32 depth) {
    auto hf = H.subset(depth);
    return LSHMap<D>(hf);
  }
  
  /** 
   * @brief Construct a @k LSHMaps with @depth hashfunctions chosen @H 
   * @param depth number of hash functions per map 
   * @param k number LSHMaps to construct
   * @returns a vector of LSHMaps constructed from random hash functions of @H
   **/
  static std::vector<LSHMap<D>> create(HashFamily<D>& H, ui32 depth, ui32 k) {
    std::vector<LSHMap<D>> ret;
    for (ui32 i = 0; i < k; ++i)
      ret.push_back(
        LSHMapFactory<D>.create(H, depth);
      );
      
    return ret;
  }
};