#include <gtest/gtest.h>

#include "../../util/ranges.hpp"
using namespace Util;

TEST(RangesVariance, CanComputeVarianceForUI32) {
  std::vector<ui32> in {1u,2u,3u,4u,5u,6u};
  double exp = 3.5,
         act = variance(ALL(in));
  ASSERT_EQ(exp, act);
}

TEST(RangesVariance, ReturnsZeroForSingle) {
  std::vector<ui32> in{1};
  double exp = 0.0,
         act = variance(ALL(in));
  ASSERT_EQ(exp, act);
}

TEST(RangesVariance, ReturnsZeroForEmptyInput) {
  std::vector<ui32> in;
  double exp = 0.0,
         act = variance(ALL(in));
  ASSERT_EQ(exp, act);
}

TEST(RangesVariance, CanComputeVarianceForDouble){
  std::vector<double> in {1.5d,2.5d,3.5d,4.5d,5.5d,6.5d};
  double exp = 3.5,
         act = variance(ALL(in));
  ASSERT_EQ(exp, act);
}


TEST(RangesVariance, CanComputeVarianceFor2DArray) {
  std::vector<std::vector<ui32>> in = { { 1, 2, 3 },
                                        { 4, 5, 6 },
                                        { 7, 8, 9 } };
  double exp = 7.5, // sample std-dev            
         act = variance2D(in); 
  ASSERT_EQ(exp, act);
}

TEST(RangesMean, CanComputeMeanFor2DArray) {
  std::vector<std::vector<ui32>> in = { { 1, 2, 3 },
                                        { 4, 5, 6 },
                                        { 7, 8, 9 } };
  double exp = 5.0,        
         act = mean2D(in); 
  ASSERT_EQ(exp, act);
}