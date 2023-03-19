#include <gtest/gtest.h>
#include "../../hash/hashfamily.hpp"
#include "../../hash/hashfamilyfactory.hpp"
#include "../../index/lshtrie.hpp"
#include "../index/util.hpp"

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

TEST(HashFamily, PairwiseMean_Correctess_Independent) {
  // Generate a hashfamily consisting of D hashfunctions that each check
  // a different bit 
  HashFamily<D> H = {
    [](const Point<D> &p) { return p[0]; },
    [](const Point<D> &p) { return p[1]; },
    [](const Point<D> &p) { return p[2]; },
    [](const Point<D> &p) { return p[3]; },
  };
  auto in = { 
    Point<D>(0b1001),
    Point<D>(0b0011),
    Point<D>(0b0110),
    Point<D>(0b1100),
  };
  
  // Each hash function evaluates to true for exactly two points in the input range                             
  // - combination (0,1) (0,2) (0,3) (2,3) evaluates to 0.25 
  // - combination (1,2) (1,3) evaluates to 0.0
  // therefore we expect..
  double exp = (0.25*4.0)/6.0; 
  ASSERT_EQ(exp, H.pairwise_mean(ALL(in)));
}

TEST(HashFamily, Mean_Correctness_Dependent) {
  HashFamily<D> H = { 
    [](const Point<D> &p) { return true; },
    [](const Point<D> &p) { return true; },
    [](const Point<D> &p) { return false; },
    [](const Point<D> &p) { return false; },
  };
  auto in = createCompleteInput();
  double exp = 0.5; // 2xfunctions evaluate true for all, and 2 false for all
  ASSERT_EQ(exp, H.mean(ALL(in)));
}

TEST(HashFamily, MeanReturns0ForEmptyInput) {
  HashFamily<D> H { [](const Point<D> &p) { return true; } };
  std::vector<Point<D>> in;
  ASSERT_EQ(0.0, H.mean(ALL(in)));
}

TEST(HashFamily, MeanReturns0ForEmptyFamily) {
  HashFamily<D> H;
  auto in = createCompleteInput();
  ASSERT_EQ(0.0, H.mean(ALL(in)));
}

TEST(HashFamily, SpreadReturns0ForEmpty) {
  HashFamily<D> H;
  auto in = createCompleteInput();
  ASSERT_EQ(0.0, H.spread(ALL(in)));
}

TEST(HashFamily, SpreadReturns0ForSingleHF) {
  HashFamily<D> H { [](const Point<D> &p) { return true; } };
  auto in = createCompleteInput();
  ASSERT_EQ(0.0, H.spread(ALL(in)));
}