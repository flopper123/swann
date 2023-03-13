#include <gtest/gtest.h>
#include "../../hash/spherichashfamily.hpp"
#include "../index/util.hpp"

TEST(HashFamily, CanConstruct_Datadependent) {
  auto in = createCompleteInput();
  SphericalHashFamily<D> H(4, in.begin(), in.end());
  ASSERT_EQ(H.size(), 4);
}