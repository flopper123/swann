#include <gtest/gtest.h>

#include "util.hpp"
#include "../../index/lsharraymap.hpp"
#include "../../index/lshforest.hpp"

TEST(LSHForestInit, CanInstantiateFromMap) {
  // Arrange
  LSHArrayMap<D>* mp = new LSHArrayMap<D>(H);
  std::vector<LSHMap<D>*> maps(1, mp);
  LSHForest<D> forest(maps);
  
  // Assert
  ASSERT_EQ(forest.size(), 0);
}

TEST(LSHForestInit, CanInstantiateFromMapAndPoints) {
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

// // Assuming there is atleast K points added to the map
// TEST(LSHForestQuery, QueryReturnsExcatlyKResults) {
//   auto in = createCompleteInput(); // Input consists of all combinations of points
//   LSHArrayMap<D>* mp = new LSHArrayMap<D>(H);
// }