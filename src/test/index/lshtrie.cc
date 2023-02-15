#include <gtest/gtest.h>
#include "../../hash/hashfamily.hpp"
#include "../../index/lshtrie.hpp"

constexpr ui32 D = 3;

HashFamily<D> H = {
  [](const Point<D>& p) { return p[0]; },
  [](const Point<D>& p) { return p[1]; },
  [](const Point<D>& p) { return p[2]; },
};

// Returns a vector containing all possible unique combinations of bitvectors
std::vector<Point<D>> createCompleteInput() {
  std::vector<Point<D>> input;

  // All combinations of input to bitset
  for (ui32 i = 0; i < (1 << D); ++i) {
    input.push_back(Point<D>(i));
  }

  return input;
}

// Add
TEST(LSHTrieTest, CanAddSinglePoint) {
  // Arrange
  std::vector<Point<D>> input = {
      Point<D>(0b000),
      Point<D>(0b001),
      Point<D>(0b010),
      Point<D>(0b011),
      Point<D>(0b100),
  };
  LSHTrie<D> trie(H);
  
  // Act
  for (auto& point : input) {
    trie.add(point);
  }

  // Assert
  ASSERT_EQ(input.size(), trie.size());
}

TEST(LSHTrieTest, CanAddBatchOfPoints) {
  // Arrange
  std::vector<Point<D>> input = {
      Point<D>(0b000),
      Point<D>(0b001),
      Point<D>(0b010),
      Point<D>(0b011),
      Point<D>(0b100),
  };
  LSHTrie<D> trie(H);
  
  // Act
  trie.add(input);
  
  // Assert
  ASSERT_EQ(input.size(), trie.size());
}

// Query
TEST(LSHTrieTest, CanQueryOnTrie) {
  // Arrange
  LSHTrie<D> trie(H);
  auto input = createCompleteInput();
  trie.add(input);
  
  for(int i = 0; i < input.size(); ++i) {
    // Act
    auto act = trie.query(input[i]);

    // Assert
    ASSERT_EQ(act.size(), 1) << "Expected 1 element in bucket but found " << act.size();
    ASSERT_EQ(act.front(), i) << "Expected bucket to contain index " << i 
                              << " but found index " << act.front();
  }
}

// Query 
TEST(LSHTrieTest, LeafNodeCanContainMultiplePoints) {
  // Arrange
  LSHTrie<D> trie(H);
  auto input = createCompleteInput();
  trie.add(input);
  
  // Act
  trie.add(Point<D>(0b101));
  std::vector<ui32> actual = trie.query(Point<D>(0b101));

  // Assert
  std::vector<ui32> expected = { 5, 8 };
  ASSERT_EQ(actual, expected);
}

//! TODO: Index mapping


// Multiple points in same bucket is returned
