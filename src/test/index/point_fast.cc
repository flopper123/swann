#include <gtest/gtest.h>
#include "../../index/point_fast.hpp"

TEST(PointFast, RandomWithP1HasAllBitsSet) {
  auto p = PointFast<1024>::Random(1.0);
  std::cout << p.wordCnt << std::endl;
  ASSERT_EQ(p.count(), 1024);
}

TEST(PointFast, RandomWithP0HasNoBitsSet) {
  auto p = PointFast<1024>::Random(0);
  ASSERT_TRUE(p.empty());
}

TEST(PointFast, RandomWithP0_5HasAboutHalfBitsSet) {
  auto p = PointFast<1024>::Random(0.5);
  ASSERT_GE(p.count(), 400);
  ASSERT_LE(p.count(), 600);
}