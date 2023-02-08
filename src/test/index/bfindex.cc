
#include <gtest/gtest.h>
// #include "util/stream.hpp"
#include "../../index/bfindex.hpp"
#include "../../dataset/parse.hpp"

TEST(BFIndexTest, IndexSizeReturnsSize) {
  BFIndex<4> index(std::vector<Point<4>> {
    Point<4>(0b0000), Point<4>(0b0001), Point<4>(0b0010), Point<4>(0b0011),
  });

  ASSERT_EQ(4, index.size());

  index.insert(Point<4>(0b0000));

  ASSERT_EQ(5, index.size());
}

TEST(BFIndexTest, LargeTest32D){

  ASSERT_TRUE(true);
}
