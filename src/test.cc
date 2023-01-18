
#include <gtest/gtest.h>

// Testing framework does not work yet

TEST(Test, BasicAssertions) {

  EXPECT_STRNE("hello", "world");

  EXPECT_EQ(7*6, 42);

}