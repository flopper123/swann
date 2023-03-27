#include <gtest/gtest.h>
#include "../../index/bfindex.hpp"

TEST(BFIndexTest, IndexSizeReturnsSize) {
  std::vector<Point<4>> input = {
      Point<4>(0b0000),
      Point<4>(0b0001),
      Point<4>(0b0010),
      Point<4>(0b0011),
  };
  BFIndex<4> index(input);

  ASSERT_EQ(4, index.size());

  auto skrt = Point<4>(0b0000);
  index.insert(skrt);

  ASSERT_EQ(5, index.size());
}


TEST(BFIndexTest, ReturnsTrueKNN) {
  // Arrange
  std::vector<Point<4>> input = {
      Point<4>(0b1100),
      Point<4>(0b1110),
      Point<4>(0b0001),
      Point<4>(0b0010),
  };
  BFIndex<4> index(input);

  Point<4> query(0b0011);
  std::vector<ui32> exp = {2, 3};
  const int k = 2;

  // Act
  auto actual = index.query(query, k);
  std::sort(ALL(actual));
  std::sort(ALL(exp));

  // Assert
  ASSERT_TRUE(std::equal(ALL(actual), ALL(exp)));
}
