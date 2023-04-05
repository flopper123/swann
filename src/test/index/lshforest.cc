#include <gtest/gtest.h>

#include "util.hpp"
#include "../../index/lsharraymap.hpp"
#include "../../index/lshforest.hpp"

TEST(LSHForestInit, CanInstantiateFromMaps) {
  // Arrange
  LSHArrayMap<D>* mp = new LSHArrayMap<D>(H);
  std::vector<LSHMap<D>*> maps(1, mp);
  LSHForest<D> forest(maps);

  // Assert
  ASSERT_EQ(forest.size(), 0);
}

TEST(LSHForestInit, CanInstantiateFromMapsAndPoints) {
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
