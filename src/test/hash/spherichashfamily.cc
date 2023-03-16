#include <gtest/gtest.h>
#include "../../hash/spherichashfamily.hpp"
#include "../index/util.hpp"

TEST(HashFamily, CanConstruct_DataDependent) {
  auto in = createCompleteInput();
  SphericalHashFamily<D> H(4, in.begin(), in.end());
  ASSERT_EQ(H.size(), 4);
}

TEST(HashFamily, CanConstruct_DataIndependent) {
  SphericalHashFamily<D> H;
  ASSERT_TRUE(H.empty());
}
