#include <gtest/gtest.h>
#include "../../hash/hashpool.hpp"

TEST(HashPoolConstructor, CanConstruct) {
  HashPool<64> pool;
  ASSERT_TRUE(!pool.empty());
}