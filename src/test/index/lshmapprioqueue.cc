#include <gtest/gtest.h>

#include "util.hpp"
#include "../../index/lshmap.hpp"
#include "../../index/lshmapfactory.hpp"
#include "../../index/lshmapprioqueue.hpp"

TEST(LSHMapPriorityQueue, CanPushAndPop) {
  LSHMapPriorityQueue<4> queue;

  LSHMap<4>* map = LSHMapFactory<4>::create(H, 1);
  
  queue.push(map);
  ASSERT_FALSE(queue.empty());

  queue.pop();
  ASSERT_TRUE(queue.empty());
}

TEST(LSHMapPriorityQueue, OrdersMapsByTheNumberOfPointsInLargestBucket) {
  LSHMapPriorityQueue<4> queue;
  
  LSHMap<4> *map1 = LSHMapFactory<4>::create(H, 3),
            *map2 = LSHMapFactory<4>::create(H, 3);

  std::vector<Point<D>> points = {
      Point<D>(0b0000),
      Point<D>(0b0010),
      Point<D>(0b0100),
      Point<D>(0b0110),
      Point<D>(0b1000),
  };

  map1->add(points);

  queue.push(map1);
  queue.push(map2);
  
  // Expect the one with most points in the largest bucket to be top
  ASSERT_EQ(queue.top()->size(), map1->size()); 
  queue.pop();

  ASSERT_EQ(queue.top()->size(), map2->size());
  queue.pop();
  
  ASSERT_TRUE(queue.empty());
}
