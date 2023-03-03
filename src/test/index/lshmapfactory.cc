#include <gtest/gtest.h>

#include "util.hpp"
#include "../../index/lshmap.hpp"
#include "../../index/lshmapfactory.hpp"

// LSHMap<D> create(HashFamily<D>& H, ui32 depth)
// std::vector<LSHMap<D>> create(HashFamily<D>& H, ui32 depth, ui32 k)

// Add
TEST(LSHMapFactoryInit, CanCreateSingleMap) {
  LSHMap<D>* map = LSHMapFactory<D>::create(H, 1);
  ASSERT_EQ(map->depth(), 1);
}

TEST(LSHMapFactoryInit, CanCreateMultipleMaps) {
  const int k = 5;
  std::vector<LSHMap<D>*> maps = LSHMapFactory<D>::create(H, 1, k);
  ASSERT_EQ(maps.size(), k);
  ASSERT_EQ(maps.front()->depth(), 1);
}
