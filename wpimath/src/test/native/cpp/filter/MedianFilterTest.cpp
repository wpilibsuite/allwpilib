// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/math/filter/MedianFilter.h>

TEST(MedianFilterTest, MedianFilterNotFullTestEven) {
  wpi::math::MedianFilter<double> filter{10};

  filter.Calculate(3);
  filter.Calculate(0);
  filter.Calculate(4);

  EXPECT_EQ(filter.Calculate(1000), 3.5);
}

TEST(MedianFilterTest, MedianFilterNotFullTestOdd) {
  wpi::math::MedianFilter<double> filter{10};

  filter.Calculate(3);
  filter.Calculate(0);
  filter.Calculate(4);
  filter.Calculate(7);

  EXPECT_EQ(filter.Calculate(1000), 4);
}

TEST(MedianFilterTest, MedianFilterFullTestEven) {
  wpi::math::MedianFilter<double> filter{6};

  filter.Calculate(3);
  filter.Calculate(0);
  filter.Calculate(0);
  filter.Calculate(5);
  filter.Calculate(4);
  filter.Calculate(1000);

  EXPECT_EQ(filter.Calculate(99), 4.5);
}

TEST(MedianFilterTest, MedianFilterFullTestOdd) {
  wpi::math::MedianFilter<double> filter{5};

  filter.Calculate(3);
  filter.Calculate(0);
  filter.Calculate(5);
  filter.Calculate(4);
  filter.Calculate(1000);

  EXPECT_EQ(filter.Calculate(99), 5);
}
