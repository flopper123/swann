#include <gtest/gtest.h>

#include "util.hpp"
#include "../../index/lsharraymap.hpp"

// Add
TEST(LSHArrayMapTest, CanAddSinglePoint) {
  // Arrange
  std::vector<Point<D>> input = {
      Point<D>(0b000),
      Point<D>(0b001),
      Point<D>(0b010),
      Point<D>(0b011),
      Point<D>(0b100),
  };
  LSHArrayMap<D> mp(H);

  // Act
  for (auto& point : input) {
    mp.add(point);
  }

  // Assert
  ASSERT_EQ(input.size(), mp.size());
}

TEST(LSHArrayMapTest, CanAddBatchOfPoints) {
  // Arrange
  std::vector<Point<D>> input = {
      Point<D>(0b000),
      Point<D>(0b001),
      Point<D>(0b010),
      Point<D>(0b011),
      Point<D>(0b100),
  };
  LSHArrayMap<D> mp(H);

  // Act
  mp.add(input);

  // Assert
  ASSERT_EQ(input.size(), mp.size());
}

// Query
TEST(LSHArrayMapTest, CanQueryForExactMatch) {
  // Arrange
  HashFamily<D> hash_fam(H.begin(), H.begin() + D);

  LSHArrayMap<D> mp(hash_fam);
  auto input = createCompleteInput();
  mp.add(input);

  for(int i = 0; i < input.size(); ++i) {
    // Act
    hash_idx bucket = mp.hash(input[i]);
    auto act = mp.query(bucket);

    // Assert
    ASSERT_EQ(act.front(), bucket);
    ASSERT_EQ(act.size(), 1) << "Expected 1 element in bucket but found " << act.size();
    ASSERT_EQ(act.front(), i) << "Expected bucket to contain index " << i 
                              << " but found index " << act.front();
  }
}

// Query 
TEST(LSHArrayMapTest, BucketCanContainMultiplePoints) {
  // Arrange
  LSHArrayMap<D> mp(H);
  auto input = createCompleteInput();
  mp.add(input);

  // Act
  mp.add(Point<D>(0b101));
  auto bucket_hash = mp.hash(Point<D>(0b101));
  std::vector<ui32> bucket = mp.query(bucket_hash);

  auto actual = mp[bucket.front()];

  // Assert
  std::vector<ui32> expected = { 5, (ui32) input.size() };
  ASSERT_EQ(actual, expected);
}
