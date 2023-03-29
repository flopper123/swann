#include <gtest/gtest.h>
#include "../../hash/dependenthashfamilyfactory.hpp"
#include "../index/util.hpp"

TEST(DependentHashFamilyFactory_HammingDistance, createsSizeHashFunctions) {
  auto in = createAllPoints<8>();
  ui32 sz = 10;
  auto HF = DependentHashFamilyFactory<8>::createHDist(ALL(in), sz);
  ASSERT_EQ(HF.size(), sz);
}
