
#include <gtest/gtest.h>
// #include "util/stream.hpp"
#include "../../index/bfindex.hpp"
#include "../../dataset/parse.hpp"

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

TEST(BFIndexTest, LargeTest32D){

  ASSERT_TRUE(true);
}
