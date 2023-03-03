#include <gtest/gtest.h>

#include "util.hpp"
#include "../../index/lsharraymap.hpp"
#include "../../index/lshforest.hpp"

TEST(LSHForest, CanInstantiate) {
  // Arrange
  LSHArrayMap<D>* mp = new LSHArrayMap<D>(H);
  std::vector<LSHMap<D>*> maps(1, mp);
  LSHForest<D> forest(maps);
  
  // Assert
  ASSERT_EQ(forest.size(), 0);
}