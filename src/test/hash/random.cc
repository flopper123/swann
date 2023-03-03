#include <gtest/gtest.h>
#include "../../hash/hashfamily.hpp"
#include "../../hash/random.hpp"
#include "../../index/lshtrie.hpp"

template<ui32 D>
auto make_point1b = []() -> Point<D>
  { return Point<D>(1ULL << ((ui64) (rand() % D))); };

/**
 * @brief
 *  1. Generate H RandomBit hash functions
 *  2. Generate all combinations of Points with 1 bit set
 *  3. Check that atleast one of the H RandomBit hash functions returns true for all points
 **/
TEST(RandomBitHash, HashChecksASingleBit) {
  // Arrange
  constexpr ui32 H_SIZE = 10, D = 3, N = 100;
  HashFamily<D> hashes;
  std::vector<Point<D>> in;
  
  for (int h = 0; h < H_SIZE; ++h) {
    hashes.push_back(RandomBit<D>());
  }

  for (int n = 0; n < N; ++n) {
    in.push_back(make_point1b<D>());
  }

  for(const auto& p : in) {
    
    // Act
    bool found = (bool) hashes(p);
    
    // Assert
    ASSERT_EQ(found, true) << "Failed find of " << p << std::endl;
  }
}

/**
 * @brief
 *  1. Generate H RandomMask hash functions
 *  2. Generate all combinations of Points with 1 bit set
 *  3. Check that atleast one of the H RandomBit hash functions returns true for all points
 **/
TEST(RandomMaskHash, HashMaskChecksAtLeastOneBit) {
  // Arrange
  constexpr ui32 H_SIZE = 32, D = 3, N = 30;
  HashFamily<D> hashes;
  std::vector<Point<D>> in;
  
  for (int h = 0; h < H_SIZE; ++h) {
    hashes.push_back(RandomMask<D>());
  }

  for (int n = 0; n < N; ++n) {
    in.push_back(make_point1b<D>());
  }

  // Act
  for (const auto &p : in)
  {
    bool found = (bool) hashes(p);

    // Assert
    ASSERT_EQ(found, true) << "Failed find of " << p << std::endl;
  }

}