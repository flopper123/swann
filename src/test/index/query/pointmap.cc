#include <gtest/gtest.h>

#include "../../../index/query/pointmap.hpp"
#include "../util.hpp"

TEST(PointMap, Get_kth_dist_ReturnsUINTMAX_If_Empty) {
  auto points = createAllPoints<10>();

  // Attempt some different k's
  for (int k = 1; k < 100; ++k) {
    PointMap<10> mp(points, Point<10>::Empty, k);
    ASSERT_EQ(mp.get_kth_dist(), UINT32_MAX);
  }
}

TEST(PointMap, Get_kth_dist_ReturnsKthNearestDistance) 
{
  Point<D> q = Point<D>::Empty;
  auto points = createCompleteInput();
  std::vector<ui32> pidxs(points.size(), 0);
  std::iota(ALL(pidxs), 0);                                       // set pidxs to 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  
  for (ui32 k = 1; k < points.size(); ++k) {
    std::shuffle(ALL(pidxs), std::mt19937{std::random_device{}()}); // shuffle pidxs to ensure random order insertion

    // Initialize exp by sorting k pidxs by hammming dist ascending
    std::vector<ui32> exps(pidxs.begin(), pidxs.begin() + k);
    std::sort(ALL(exps), [&points, &q](const ui32& i, const ui32& j){
      return points[i].distance(q) < points[j].distance(q);
    }); 

    // Initialize act
    PointMap<D> mp(points, q, k);
    mp.insert(pidxs.begin(), pidxs.begin() + k);
    
    ui32 act = mp.get_kth_dist(),
         exp = exps.empty() ? UINT32_MAX : points[exps.back()].distance(q);

    ASSERT_EQ(exps.size(), mp.size());
    ASSERT_EQ(act, exp);
  }
}

TEST(PointMap, GetKNearest_ReturnsAtleastK)
{
  Point<D> q = Point<D>::Empty;
  auto points = createCompleteInput();
  std::vector<ui32> pidxs(points.size(), 0);
  std::iota(ALL(pidxs), 0);                                       // set pidxs to 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  
  for (ui32 k = 1; k < points.size() - 1; ++k) {
    // Query for k, but insert k+1
    PointMap<D> mp(points, q, k);
    std::shuffle(ALL(pidxs), std::mt19937{std::random_device{}()}); // shuffle pidxs to ensure random order insertion
    mp.insert(pidxs.begin(), pidxs.begin() + k  + 1);
    auto act = mp.extract_k_nearest();
    ASSERT_EQ(act.size(), k) << "Expected extract_k_nearest to return k points, but got " << act.size() << " instead";
  }
}

TEST(PointMap, GetKNearest_ReturnsUpToK)
{
  Point<D> q = Point<D>::Empty;
  auto points = createCompleteInput();
  std::vector<ui32> pidxs(points.size(), 0);
  std::iota(ALL(pidxs), 0);                                       // set pidxs to 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  
  for (ui32 k = 1; k < points.size(); ++k) {
    std::shuffle(ALL(pidxs), std::mt19937{std::random_device{}()}); // shuffle pidxs to ensure random order insertion
    PointMap<D> mp(points, q, k);
    mp.insert(pidxs.begin(), pidxs.begin() + k);
    auto act = mp.extract_k_nearest();
    ASSERT_EQ(act.size(), k) << "Expected extract_k_nearest to return k points, but got " << act.size() << " instead";
  }
}

TEST(PointMap, GetKNearest_ReturnsKNNInAscendingOrder)
{
  ui32 k = 10;
  auto points = createCompleteInput();
  std::vector<ui32> pidxs(points.size(), 0);
  std::iota(ALL(pidxs), 0); // set pidxs to 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  std::shuffle(ALL(pidxs), std::mt19937{std::random_device{}()}); // shuffle pidxs to ensure random order insertion

  Point<D> q = Point<D>::Empty;
  PointMap<D> mp(points, q, k);
  mp.insert(pidxs.begin(), pidxs.end());
  std::vector<ui32> exps(pidxs.begin(), pidxs.end());
  std::sort(ALL(exps), [&points, &q](const ui32& i, const ui32& j){
    return q.distance(points[i]) < q.distance(points[j]);
  }); 

  std::vector<ui32> knn = mp.extract_k_nearest();  
  ASSERT_GE(exps.size(), knn.size());
  for(int i = 0; i < knn.size(); ++i) {
    ui32 act = q.distance(points[knn[i]]), 
         exp = q.distance(points[exps[i]]);
    
    ASSERT_EQ(act, exp) 
      << "Expected equality of " << i << "th elements:\n\t" 
      << "act : " << act << " which is point{" << points[knn[i]] << "}\n\t"
      << "exp : " << exp << " which is point{" << points[exps[i]] << "}" << std::endl;
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
