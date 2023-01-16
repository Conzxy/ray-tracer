#include "img/color.hh"

#include <gtest/gtest.h>

using namespace img;

TEST (color_test, test1) {
  Color c(123,122,233);

  EXPECT_EQ(c[0], 233);
  EXPECT_EQ(c[1], 122);
  EXPECT_EQ(c[2], 123);
}

