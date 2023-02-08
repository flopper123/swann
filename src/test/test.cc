#include <gtest/gtest.h>

TEST(Test, BasicAssertions) {

  EXPECT_STRNE("hello", "world");
  EXPECT_EQ(7*6, 42);

}

TEST(Test, BasicAssertionsFalse)
{
  EXPECT_EQ(7 * 6, 42);
  EXPECT_EQ(7 * 6, 42);
}
