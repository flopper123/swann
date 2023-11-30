#include <gtest/gtest.h>

#include "util.hpp"
#include "../../index/lshhashmap.hpp"

// Add
TEST(LSHHashMapTest, CanAddSinglePoint) {
  // Arrange
  std::vector<Point<D>> input = {
      Point<D>(0b000),
      Point<D>(0b001),
      Point<D>(0b010),
      Point<D>(0b011),
      Point<D>(0b100),
  };
  LSHHashMap<D> mp(H);

  // Act
  for (auto& point : input) {
    mp.add(point);
  }

  // Assert
  ASSERT_EQ(input.size(), mp.size());
}

TEST(LSHHashMapTest, CanAddBatchOfPoints) {
  // Arrange
  std::vector<Point<D>> input = {
      Point<D>(0b000),
      Point<D>(0b001),
      Point<D>(0b010),
      Point<D>(0b011),
      Point<D>(0b100),
  };
  LSHHashMap<D> mp(H);

  // Act
  mp.add(input);

  // Assert
  ASSERT_EQ(input.size(), mp.size());
}

// Query
TEST(LSHHashMapTest, CanQueryForExactMatch) {
  // Arrange
  HashFamily<D> hash_fam(H.begin(), H.begin() + D);

  LSHHashMap<D> mp(hash_fam);
  auto input = createCompleteInput();
  mp.add(input);
  mp.optimize();

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
TEST(LSHHashMapTest, BucketCanContainMultiplePoints) {
  // Arrange
  auto p = Point<D>(0b101);
  LSHHashMap<D> mp(H);
  auto input = createCompleteInput();
  mp.add(input);
  mp.add(p);
  mp.optimize();

  // Act
  auto bucket_hash = mp.hash(p);
  std::vector<ui32> bucket = mp.query(bucket_hash);
  std::vector<ui32> actual = (std::vector<ui32>)mp[bucket.front()];

  // Assert
  std::vector<ui32> expected = { 5, (ui32) input.size() };
  ASSERT_EQ(actual, expected);
}

// Query
TEST(LSHHashMapTest, QueryReturnsCorrectHammingDistanceOfBucketId)
{
  // Arrange
  LSHHashMap<D> mp(H);
  Point<D> p(0b101);
  auto bucket_hash = mp.hash(p);
  Point<32> bucket_hash_point(bucket_hash);
  auto input = createCompleteInput();
  mp.add(input);
  mp.optimize();
  
  // Act
  for (int i = 0; i < H.size(); ++i) {
    std::vector<ui32> buckets = mp.query(bucket_hash, i);
    
    // Assert
    for (auto& bucket : buckets) {
      Point<32> bucket_point(bucket);
      ASSERT_EQ(bucket_point.distance(bucket_hash_point), i);
    }
  }
}
