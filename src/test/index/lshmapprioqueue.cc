#include <gtest/gtest.h>

#include "util.hpp"
#include "../../index/lshmap.hpp"
#include "../../index/bucketmask.hpp"
#include "../../index/lshmapfactory.hpp"

TEST(LSHMapPriorityQueue, CanPush) {
  BucketMask masks(1);
  LSHMapPriorityQueue<4> queue(H, masks, 1, 1);

  LSHMap<4>* map = LSHMapFactory<4>::create(H, masks, 1);
  
  queue.push(map);
  ASSERT_FALSE(queue.empty());
}

TEST(LSHMapPriorityQueue, QueueIsInitiallyEmpty) {
  const ui32 k = 2, depth = 1;
  BucketMask masks(depth);
  LSHMapPriorityQueue<4> queue(H, masks, k, depth);
  ASSERT_TRUE(queue.empty());
}

TEST(LSHMapPriorityQueue, GetAllReturnsMaxSizeMaps) {
  const ui32 k = 2, depth = 1;
  BucketMask masks(1);
  LSHMapPriorityQueue<4> queue(H, masks, k, depth);
  LSHMap<4>* m1 = LSHMapFactory<4>::create(H, masks, depth),
           * m2 = LSHMapFactory<4>::create(H, masks, depth);

  queue.push(m1);
  queue.push(m2);
  ASSERT_FALSE(queue.empty());
  
  std::vector<LSHMap<4>*> maps = queue.get_all();
  ASSERT_EQ(maps.size(), k);
}

TEST(LSHMapPriorityQueue, PushDoesNotAddMap_IfFullAndMorePointsInLargestBucket) {
  const ui32 k = 2, depth = 3;
  BucketMask masks(depth);
  LSHMapPriorityQueue<4> queue(H, masks, k, depth);
  
  LSHMap<4> *empty1 = LSHMapFactory<4>::create(H, masks, depth),
            *empty2 = LSHMapFactory<4>::create(H, masks, depth),
            *mp = LSHMapFactory<4>::create(H, masks, depth);

  std::vector<Point<D>> points = {
      Point<D>(0b0000),
      Point<D>(0b0010),
      Point<D>(0b0100),
      Point<D>(0b0110),
      Point<D>(0b1000),
  };

  mp->add(points);

  ASSERT_TRUE(queue.push(empty1));
  ASSERT_TRUE(queue.push(empty2));
  ASSERT_FALSE(queue.push(mp)); // Should not be added
}

TEST(LSHMapPriorityQueue, OrdersMapsByTheNumberOfPointsInLargestBucket) {
  BucketMask masks(3);
  LSHMapPriorityQueue<4> queue(H, masks, 2, 3);
  
  LSHMap<4> *map1 = LSHMapFactory<4>::create(H, masks, 3),
            *map2 = LSHMapFactory<4>::create(H, masks, 3);

  std::vector<Point<D>> points = {
      Point<D>(0b0000),
      Point<D>(0b0010),
      Point<D>(0b0100),
      Point<D>(0b0110),
      Point<D>(0b1000),
  };

  map1->add(points);
  ASSERT_TRUE(queue.push(map1));  
  ASSERT_TRUE(queue.push(map2));

  // Expect the one with most points in the largest bucket to be top
  ASSERT_EQ(queue.top()->maxBucketSize(), map1->maxBucketSize());
}
