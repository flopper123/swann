#include <gtest/gtest.h>

#include "../../../index/query/pointmap.hpp"
#include "../util.hpp"

TEST(PointMap, Get_kth_dist_ReturnsUINTMAX_If_Empty) {
  auto points = createCompleteInput();
  std::vector<ui32> pidxs(points.size(), 0);
  std::iota(ALL(pidxs), 0); // set pidxs to 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  Point<D> q = Point<D>::Empty;
  PointMap<D> mp(points, q);
  ASSERT_EQ(mp.get_kth_dist(0), UINT32_MAX);
}

TEST(PointMap, Get_kth_dist_ReturnsKthNearestDistance) 
{
  auto points = createCompleteInput();
  std::vector<ui32> pidxs(points.size(), 0);
  std::iota(ALL(pidxs), 0); // set pidxs to 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  Point<D> q = Point<D>::Empty;
  PointMap<D> mp(points, q);
  mp.insert(pidxs.begin(), pidxs.end());

  std::vector<ui32> exp = {0, 1, 1, 1, 1, 2};
  for (int i = 0; i < exp.size(); ++i)
  {
    ui32 act = mp.get_kth_dist(i);
    ASSERT_EQ(act, exp[i]) << "Expected get_kth_dist(" << i << ") to return " << exp[i] << ", but got " << act << " instead";
  }
}

TEST(PointMap, GetKNearest_ReturnsMaximumSizeofMap)
{
  auto points = createCompleteInput();
  std::vector<ui32> pidxs(points.size(), 0);
  std::iota(ALL(pidxs), 0);                                       // set pidxs to 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  std::shuffle(ALL(pidxs), std::mt19937{std::random_device{}()}); // shuffle pidxs to ensure random order insertion
  Point<D> q = Point<D>::Empty;
  PointMap<D> mp(points, q);
  auto act = mp.get_k_nearest(10);
  ASSERT_EQ(act.size(), 0) << "Expected get_k_nearest to return no points, but got " << act.size() << " instead";
  mp.insert(pidxs.begin(), pidxs.end());
  act = mp.get_k_nearest(10);
  ASSERT_EQ(act.size(), 10);
}

TEST(PointMap, GetKNearest_ReturnsKNN)
{
  auto points = createCompleteInput();
  std::vector<ui32> pidxs(points.size(), 0);
  std::iota(ALL(pidxs), 0); // set pidxs to 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  std::shuffle(ALL(pidxs), std::mt19937{std::random_device{}()}); // shuffle pidxs to ensure random order insertion
  Point<D> q = Point<D>::Empty;
  
  PointMap<D> mp(points, q);
  mp.insert(pidxs.begin(), pidxs.end());

  std::vector<ui32> exp_dist = {0, 1, 1, 1, 1, 2},
                    knn = mp.get_k_nearest(6);  

  for(int i = 0; i < knn.size(); ++i) {
    ui32 act = q.distance(points[knn[i]]),
         exp = exp_dist[i];
    ASSERT_EQ(act, exp) << "Expected point[" << i << "] to be distance of " 
                         << exp << ", but got " 
                         << act << " instead";
  }
}

TEST(PointMap, Contains_TrueIfContained_FalseIfNot) {
  auto points = createCompleteInput();
  std::vector<ui32> pidxs(points.size(), 0);
  std::iota(ALL(pidxs), 0); // set pidxs to 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  std::shuffle(ALL(pidxs), std::mt19937{std::random_device{}()}); // shuffle pidxs to ensure random order insertion
  Point<D> q = Point<D>::Empty;
  PointMap<D> mp(points, q);

  for(int i = 0; i < pidxs.size(); ++i) {
    ASSERT_FALSE(mp.contains(pidxs[i])) << "Expected point[" << i << "] to be contained, but it was not";
    mp.insert(pidxs[i]);
    ASSERT_TRUE(mp.contains(pidxs[i])) << "Expected point[" << i << "] to be contained, but it was not";
  }
}
