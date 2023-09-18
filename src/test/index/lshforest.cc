#include <gtest/gtest.h>

#include "util.hpp"
#include "../../index/lsharraymap.hpp"
#include "../../index/lshforest.hpp"

TEST(LSHForestInit, CanInstantiate) {
  // Arrange
  LSHArrayMap<D>* mp = new LSHArrayMap<D>(H);
  std::vector<LSHMap<D>*> maps(1, mp);
  std::vector<Point<D>> points = createCompleteInput();

  LSHForest<D> forest(maps, points);

  // Assert
  ASSERT_EQ(forest.size(), points.size());
}

TEST(LSHForestBuild, CanBuild) {
  // Arrange
  LSHArrayMap<D>* mp = new LSHArrayMap<D>(H);
  std::vector<LSHMap<D>*> maps(1, mp);
  std::vector<Point<D>> points = createCompleteInput();

  LSHForest<D> forest(maps, points);
  forest.build();

  // Assert
  ASSERT_EQ(forest.size(), points.size());
}

TEST(LSHForestBuild, BuildInsertPointsIntoAllMaps) {
  // Arrange
  std::vector<LSHMap<D> *> maps { new LSHArrayMap<D>(H), new LSHArrayMap<D>(H) };
  std::vector<Point<D>> points = createCompleteInput();

  LSHForest<D> forest(maps, points);
  forest.build();

  // Assert
  for (auto& mp : forest.getMaps())
    ASSERT_EQ(mp->size(), points.size());
}

// Expect excatly K results and correct distance
TEST(LSHForestQuery, QueryReturnsCorrectResults) {
  // Arrange : Build all maps on all combinations of points  
  std::vector<LSHMap<D>*> maps = LSHMapFactory<D>::create(H, 2, 2);
  std::vector<Point<D>> points = createCompleteInput();
  LSHForest<D> forest(maps, points);
  forest.build();
  
  // Act : Query for the single nearest neighbour for all points inserted 
  for (auto& p : points) 
  {
    ui32 k = 0;
    for (ui32 bits = 0; bits < D; ++bits) {
      k += (1UL << bits);
      auto kNearestNeighbours = forest.query(p, k);

      ASSERT_EQ(kNearestNeighbours.size(), k);

      for (auto &pidx : kNearestNeighbours)
      {
        auto p2 = forest[pidx];
        ASSERT_LE(p.distance(p2), bits+1) << "Query(" << p << ", " << k << ") :: Found " << kNearestNeighbours.size() << " results\n"
                                          << "Expected hamming distance of " << bits << ". "
                                          << "Received << " << p.distance(p2) << std::endl;
      }
    }
  }
}

// Asserts correct ordering of outputs and that the results correspond to the result of the single-threaded query
TEST(LSHForestQuery, MThread_QueryReturnsCorrectResults) {
  const ui32 Q = 10, k = 3, thread_cnt = std::thread::hardware_concurrency();
  const float recall = 1.0;

  // Arrange : Generate random query points and build index on all combination of points
  std::vector<Point<D>> queries; // Q random query points
  std::vector<LSHMap<D>*> maps = LSHMapFactory<D>::create(H, 2, 2);
  std::vector<Point<D>> points = createCompleteInput();
  LSHForest<D> forest(maps, points);
  forest.build();
  for (ui32 q=0; q < Q; ++q) {
    queries.emplace_back(Point<D>::random());
  }

  // Act : Query for the nearest neighbours among all points inserted
  std::vector<std::vector<ui32>> mt_results = forest.mthread_queries(queries, k, recall, thread_cnt);

  // Assert : Compare mt_results with mt_results of single threaded query
  for (ui32 q=0; q < Q; ++q) {
    auto st_result = forest.query(queries[q], k, recall);
    ASSERT_EQ(mt_results[q].size(), st_result.size());
    for (ui32 i=0; i < mt_results[q].size(); ++i) {
      ASSERT_EQ(mt_results[q][i], st_result[i]);
    }
  }
}