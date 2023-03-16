#include <gtest/gtest.h>
#include "../../index/pointarithmeticresult.hpp"

TEST(PointSubtraction, CanConstruct) {
  auto p1 = random_point<4>(), p2 = random_point<4>();
  PointSubtractionResult<4> res(p1, p2);
}

TEST(PointSubtraction, SubtractsCorrectly) {
  auto p1 = Point<4>(0b0101), p2 = Point<4>(0b0101);
  PointSubtractionResult<4> res(p1, p2);
}