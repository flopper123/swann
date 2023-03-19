#include <gtest/gtest.h>
#include "../../hash/spherichashfamily.hpp"
#include "../index/util.hpp"

TEST(SphericHashFamily, CanConstruct_DataDependent) {
  auto in = createCompleteInput();
  SphericalHashFamily<D> H(4, in.begin(), in.end());
  ASSERT_EQ(H.size(), 4);
}

TEST(SphericHashFamily, CanConstruct_DataIndependent) {
  SphericalHashFamily<D> H;
  ASSERT_TRUE(H.empty());
}

// TEST(SphericHashFamily, DataDependent_Optimization_Creates_HF_LE_MeanErrorN4) {
//   SphericHashFamily<D> hf;
//   double err_mean = 0.10;
//   auto in = createCompleteInput();
//   // The pair-wise mean of the returned hashfamily is expected to be
//   hf.optimize(4, ALL(in), err_mean);

//   ASSERT_LE(hf.pairwise_mean(ALL(in)), err_mean * (in.size() / 4));
// }

// TEST(SphericHashFamily, DataDependent_Optimization_Creates_HF_with_spread_le_StdErrorN4) {
//   SphericHashFamily<D> hf;
//   double err_std_dev = 0.15, err_mean = 0.1;
//   auto in = createCompleteInput();
//   // The pair-wise mean of the returned hashfamily is expected to be
//   hf.optimize(4, ALL(in), err_mean, err_std_dev); 
//   ASSERT_LE(hf.spread(ALL(in)), err_std_dev * (in.size() / 4));
// }