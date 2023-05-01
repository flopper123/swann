#pragma once

#include "../index/lshmap.hpp"

template<ui32 D>
class LSHMapAnalyzer {
public:
  LSHMapAnalyzer(LSHMap<D>* lshMap) {
    this->lshMap = lshMap;
  }
  
  std::vector<ui32> getBucketDistribution() {
    std::vector<ui32> res = lshMap->get_bucket_sizes();

    // sort the vector from high to low
    std::sort(res.begin(), res.end(), std::greater<ui32>());

    return res;
  }

private:
  LSHMap<D> *lshMap;
  
};