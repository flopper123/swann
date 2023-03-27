#pragma once

#include "../util/ranges.hpp"
#include "../index/lshforest.hpp"
#include "lshmapanalyzer.hpp"

template<ui32 D>
class StatsGenerator {
  StatsGenerator() {}

public:
  
  static void runStats(LSHForest<D> *index) {

    // Bucket distribution
    std::cout << "[+] Bucket Distribution:" << std::endl;
    for (auto map : index->getMaps()) {
      LSHMapAnalyzer<D> analyzer(map);
      auto distribution = analyzer.getBucketDistribution();

      std::cout << "Bucket: " << std::endl;
      for (ui32 bucket : distribution) {
        std::cout << bucket << " ";
      }
      std::cout << std::endl;
      std::cout << "variance: " << Util::variance(ALL(distribution)) << std::endl;
      std::cout << "std. dev.: " << Util::std_dev(ALL(distribution)) << std::endl;
      std::cout << "mean: " << Util::mean(ALL(distribution)) << std::endl;
      
      std::cout << std::endl;
    }
  }
};