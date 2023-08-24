#include <gtest/gtest.h>
#include "../../index/pointarithmeticresult.hpp"

TEST(PointSubtraction, CanConstruct) {
  auto p1 = Point<4>::random(), p2 = Point<4>::random();
  auto [pos,neg] = PointCalculator::subtraction<4>(p1, p2);
}

TEST(PointSubtraction, SamePoint_ReturnsEmpty) {
  auto p1 = Point<4>(0b0101), p2 = Point<4>(0b0101);
  auto [pos,neg] = PointCalculator::subtraction<4>(p1, p2);
  ASSERT_EQ(pos, Point<4>::Empty) << "Expected pos() to be empty, but actually is " << pos;
  ASSERT_EQ(neg, Point<4>::Empty) << "Expected neg() to be empty, but actually is " << neg;
}

TEST(PointSubtraction, DifferentPoint_PosIsBitsOnlyInP1) {
  auto p1 = Point<4>(0b1000), p2 = Point<4>(0b0101);
  auto [pos,neg] = PointCalculator::subtraction<4>(p1, p2);
  ASSERT_EQ(pos, p1);
}

TEST(PointSubtraction, DifferentPoint_NegIsBitsOnlyInP2) {
  auto p1 = Point<4>(0b1000), p2 = Point<4>(0b0101);
  auto [pos,neg] = PointCalculator::subtraction<4>(p1, p2);
  ASSERT_EQ(neg, p2);
}
