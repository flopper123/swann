#include <gtest/gtest.h>


#include "../../index/point.hpp"

constexpr ui32 MAX_D = 10000;

TEST(Point, SphericalDistDivision_TransformingToSameDecreasesDistance) {
  Point<MAX_D> p1(0x0), p2(0x0);
  p1.flip(); // all bits are now set
  
  ASSERT_NE(p1,p2);
  
  for (double d1, d2 = DBL_MAX, i = 0;
       p1 != p2;
       d2 = d1)
  {
    d1 = p1.spherical_distance(p2);
    ASSERT_LT(d1, d2);
    p2[i++] = true;
  }
}

// To allow conversion from spherical_distance double to uint32 we multiply by 100 and round
TEST(Point, MaxSphericalDistDivision_LE_UINT32MAX_DIV100) {
  Point<MAX_D> p1(0x0), p2 = (0x0);
  p1.flip();
  ASSERT_LE(p1.spherical_distance(p2), (double) UINT32_MAX/100.0);
}

TEST(Point, SphericalDistDivision_SameIsLEPointOffset) {
  Point<4> p1(0b1111), p2(0b1111);
  ASSERT_EQ(p1.spherical_distance(p2), 0.0);
}

TEST(Point, SphericalDistDivision_NoDivisionBy0Error_IfPointsAreDisjoint) {
  Point<4> p1(0b0000), p2(0b1111);
  ASSERT_GT(p1.spherical_distance(p2), 1.0);
}