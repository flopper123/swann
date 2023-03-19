#include <gtest/gtest.h>
#include "../../index/pointarithmeticresult.hpp"

TEST(PointSubtraction, CanConstruct) {
  auto p1 = random_point<4>(), p2 = random_point<4>();
  PointSubtractionResult<4> res(p1, p2);
}

TEST(PointSubtraction, SamePoint_ReturnsEmpty) {
  auto p1 = Point<4>(0b0101), p2 = Point<4>(0b0101);
  PointSubtractionResult<4> res(p1, p2);
  ASSERT_EQ(res.pos(), Point<4>::Empty) << "Expected pos() to be empty, but actually is " << res.pos();
  ASSERT_EQ(res.neg(), Point<4>::Empty) << "Expected neg() to be empty, but actually is " << res.neg();;
}

TEST(PointSubtraction, DifferentPoint_PosIsBitsOnlyInP1) {
  auto p1 = Point<4>(0b1000), p2 = Point<4>(0b0101);
  PointSubtractionResult<4> res(p1, p2);
  ASSERT_EQ(res.pos(), p1);
}

TEST(PointSubtraction, DifferentPoint_NegIsBitsOnlyInP2) {
  auto p1 = Point<4>(0b1000), p2 = Point<4>(0b0101);
  PointSubtractionResult<4> res(p1, p2);
  ASSERT_EQ(res.neg(), p2);
}
