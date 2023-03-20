#include <gtest/gtest.h>
#include "../../hash/sphericalhashfamily.hpp"
#include "../index/util.hpp"

TEST(SphericalHashFamily, CanConstruct_DataDependent) {
  auto in = createCompleteInput();
  SphericalHashFamily<D> H(4, in.begin(), in.end());
  ASSERT_EQ(H.size(), 4);
}

TEST(SphericalHashFamily, CanConstruct_DataIndependent) {
  SphericalHashFamily<D> H;
  ASSERT_TRUE(H.empty());
}

TEST(SphericalHashFamily, DataDependent_Optimization_Creates_HF_LE_MeanErrorN4) {
  SphericalHashFamily<D> hf;
  double err_mean = 0.10;
  auto in = createCompleteInput();
  // The pair-wise mean of the returned hashfamily is expected to be
  hf.optimize(4, ALL(in), err_mean);
  std::cout << hf.str(ALL(in)) << std::endl;
  ASSERT_LE(hf.pairwise_mean(ALL(in)), err_mean * (in.size() / 4));
}

// TEST(SphericalHashFamily, DataDependent_Optimization_Creates_HF_with_spread_le_StdErrorN4) {
//   SphericalHashFamily<D> hf;
//   double err_std_dev = 0.15, err_mean = 0.1;
//   auto in = createCompleteInput();
//   // The pair-wise mean of the returned hashfamily is expected to be
//   hf.optimize(4, ALL(in), err_mean, err_std_dev); 
//   std::cout << hf.str(ALL(in)) << std::endl;
//   ASSERT_LE(hf.spread(ALL(in)), err_std_dev * (in.size() / 4));
// }