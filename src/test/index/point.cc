#include <gtest/gtest.h>
#include "../../index/point.hpp"

TEST(Point, RandomWithP1HasAllBitsSet) {
  auto p = Point<1024>::Random(1.0);
  std::cout << p.wordCnt << std::endl;
  ASSERT_EQ(p.count(), 1024);
}

TEST(Point, RandomWithP0HasNoBitsSet) {
  auto p = Point<1024>::Random(0);
  ASSERT_TRUE(p.empty());
}

TEST(Point, RandomWithP0_5HasAboutHalfBitsSet) {
  auto p = Point<1024>::Random(0.5);
  ASSERT_GE(p.count(), 400);
  ASSERT_LE(p.count(), 600);
}