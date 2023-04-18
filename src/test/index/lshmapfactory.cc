#include <gtest/gtest.h>

#include "util.hpp"
#include "../../index/lshmap.hpp"
#include "../../index/lshmapfactory.hpp"

// Add
TEST(LSHMapFactoryInit, CanCreateSingleMap) {
  LSHMap<D>* map = LSHMapFactory<D>::create(H, 1);
  ASSERT_EQ(map->depth(), 1);
}

TEST(LSHMapFactoryInit, CanCreateMultipleMaps) {
  const int k = 4;
  std::vector<LSHMap<D>*> maps = LSHMapFactory<D>::create(H, 1, k);
  ASSERT_EQ(maps.size(), k);
  ASSERT_EQ(maps.front()->depth(), 1);
}
