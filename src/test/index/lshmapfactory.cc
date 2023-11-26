#include <gtest/gtest.h>

#include "util.hpp"
#include "../../index/lshmap.hpp"
#include "../../index/bucketmask.hpp"
#include "../../index/lshmapfactory.hpp"

// Add
TEST(LSHMapFactoryInit, CanCreateSingleMap) {
  BucketMask masks(1);
  LSHMap<D>* map = LSHMapFactory<D>::create(H, masks, 1);
  ASSERT_EQ(map->depth(), 1);
}

TEST(LSHMapFactoryInit, CanCreateMultipleMaps) {
  const int k = 4;
  BucketMask masks(1);
  std::vector<LSHMap<D>*> maps = LSHMapFactory<D>::create(H, masks, 1, k);
  ASSERT_EQ(maps.size(), k);
  ASSERT_EQ(maps.front()->depth(), 1);
}

TEST(LSHMapFactoryThreadedTrieRebuilding, ReturnsExcatlyKMaps) {
  const int k = 4, steps = 3;
  auto points = createCompleteInput();
  std::vector<LSHMap<D> *> maps = LSHMapFactory<D>::mthread_create_optimized(points, H, 1, k, steps);
  ASSERT_EQ(maps.size(), k);
  ASSERT_EQ(maps.front()->depth(), 1);
}