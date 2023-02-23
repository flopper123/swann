#include <gtest/gtest.h>
#include "../../hash/hashfamily.hpp"
#include "../../index/lshtrie.hpp"

TEST(HashFamily, HashSetsTheIthBit_ToResultOfIthHash) {
  ui64 depth = 64;
  HashFamily<64> HTrue, 
                 HFalse(depth, [](const Point<64> &p) { return false; });

  for (ui64 i = 0; i < depth; ++i) {
    HTrue.push_back(([i](const Point<64> &p){ return p[i]; }));
  }
  ASSERT_EQ(64ULL, HTrue.size());

  for (ui64 i = 0; i < depth; ++i)
  {
    ui64 exp = 1ULL << i;
    auto p = Point<64>(exp);
    ui64 act = HTrue(p);
    // We expect only the i'th bit to be setwhen applying HTrue
    ASSERT_EQ(exp, act) << "Ran test on <" << p << ">\n"
                        << "Expected: " << p << "\n" 
                        << "Actual:   " << Point<64>(act) << std::endl;
    // We expect no bits to be set when applying HFalse
    ASSERT_EQ(Point<64>(0x0), HFalse(p));
  }
}
