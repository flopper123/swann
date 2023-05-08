#pragma once

#include "../util/ranges.hpp"
#include "../index/lshforest.hpp"
#include "lshmapanalyzer.hpp"

template<ui32 D>
class StatsGenerator {
  StatsGenerator() {}

public:
  
  static void runStats(LSHForest<D> *index) {
    double acc_var = 0, acc_std_dev = 0;
    // Bucket distribution
    std::cout << "[+] Bucket Distribution:" << std::endl;
    int m=0;
    for (auto map : index->getMaps()) {
      LSHMapAnalyzer<D> analyzer(map);
      auto distribution = analyzer.getBucketDistribution();

      std::cout << "LSHMap[" << ++m << "] with " << distribution.size() 
                << " buckets" << std::endl << "\tdistribution: " << std::endl;
      // int i=0;
      // for (ui32 bucket : distribution) {
      //   ++i;
      //   if (bucket == 0)
      //   { 
      //     std::cout << "0*" << (distribution.size() - i);
      //     break;
      //   }
      //   std::cout << bucket << " ";
      // }
      // std::cout << std::endl;
      
      double var = Util::variance(ALL(distribution)),
             std_dev = Util::std_dev(ALL(distribution));

      std::cout << "\tvariance: " << var << "\n"
                << "\tstd. dev.: " << std_dev << "\n";
      acc_var += var;
      acc_std_dev += std_dev;
    }
      
    std::cout << "[+] Overall Statistics:\n"
              << "\tVariance: " << acc_var / index->getMaps().size() << std::endl
              << "\tStd. Dev.: " << acc_std_dev / index->getMaps().size() << std::endl;
  }
};